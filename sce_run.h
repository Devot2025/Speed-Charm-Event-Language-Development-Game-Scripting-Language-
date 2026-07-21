#ifndef _SCE_RUN_H_
#define _SCE_RUN_H_
#include "sce_ir.h"

/**
* Memory Management Design
* 
* Store all reference information in a dedicated memory region within the VM.
* When an error occurs, inspect the VM's registers, stack, and variable memory to determine which resources should be released.
* If an address is referenced only by a register, release the register and deallocate the associated memory, since no other references exist.
* If an address is referenced by both a variable and a register, release only the variable. Keep the register and the associated memory allocated because the register still holds a valid reference.
*
*/
#ifdef GAME_VECTOR_LIB
#include "game_vector_lib.h"
#endif
#define auto_struct(Name, Implemenet) typedef struct Name{Implemenet}Name

typedef enum Sce_Run_Value_Type {
	E_Sce_Run_Null_Value,
	E_Sce_Run_Virtual_Value,
	E_Sce_Run_Bool_Value,
	E_Sce_Run_Char_Value,
	E_Sce_Run_Int_Value,
	E_Sce_Run_Float_Value,
	E_Sce_Run_Double_Value,
	E_Sce_Run_String_Value,
	E_Sce_Run_Obj_Value,
	E_Sce_Run_Mem_Value,
}Sce_Run_Value_Type;
typedef enum Sce_Run_Ext_Value_Type {
	E_Sce_Run_Ext_Null_Value,
	E_Sce_Run_Ext_FILE_Value,
}Sce_Run_Ext_Value_Type;
static const uint8_t* run_value_type_str[] =
{ "null", "virtual", "bool", "char", "int", "float", "double", "string", "obj", "mem" };

typedef void *(sce_free_func_t)(void*);
typedef sce_free_func_t* sce_free_func_ptr_t;
typedef struct Sce_Ext_Values {
	void* ptr__;
	Sce_Run_Ext_Value_Type ext_type;
	//sce_free_func_ptr_t sce_free;
}Sce_Ext_Values;
typedef struct Sce_Run_Values{
	Sce_Run_Value_Type value_type;
	union {
		void* vvalue;
		int64_t ivalue;
		U8_String_Buffers* ssvalue;
		Sce_Ext_Values* evalue;
		uint8_t cvalue;
		float fvalue;
		double dvalue;
		bool bvalue;
		struct Sce_Virtual_Memory* mvalue;
	};
}Sce_Run_Values;

#define SCE_VM_FALLBACK_CODE 1
#define IS_CONST_MEMORY true
#define NOT_IS_CONST_MEMORY false
typedef struct Sce_Virtual_Memory {
	bool is_const_value;
	Sce_Run_Values sce_run_value;
}Sce_Virtual_Memory;

typedef struct Sce_Run_Objects {
	Sce_Virtual_Memory* sce_vmem;
}Sce_Run_Objects;
typedef Sce_Virtual_Memory Sce_Virutal_Register;

typedef struct Sce_Virutal_Registers {
	Sce_Virutal_Register svr[SCE_VIRTUAL_REGISTER_MAX + 2];
}Sce_Virutal_Registers;

typedef struct Sce_Virtual_Stack {
	struct Sce_Virtual_Stack* next;
	Sce_Virtual_Memory svm;
}Sce_Virtual_Stack;

typedef struct Sce_VM_Label_Table {
	struct Sce_VM_Label_Table* next_;
	Sce_Binary_Machine_Instructions* lebel_adress_;
}Sce_VM_Label_Table;
struct Sce_VM_Object_Table;
typedef struct Sce_Run_VM_Tables {
	Sce_VM_Label_Table* label_table_;
	struct Sce_VM_Object_Table* obj_table_;
	Sce_VM_Label_Table* init_label_;
}Sce_Run_VM_Tables;
typedef struct Sce_VM_Label_Table_Context {
	//Sce_Run_VM_Tables symbol_table;
	/*next label*/
	struct Sce_VM_Object_Table* obj_table_;
	struct Sce_VM_Label_Table_Context* next_;
	Sce_Binary_Machine_Instructions* lebel_adress_;
}Sce_VM_Label_Table_Context;
typedef struct Sce_VM_Object_Table {
	struct Sce_VM_Object_Table* next_;
	uint8_t* obj_name_;
	Sce_Virtual_Memory svm;
}Sce_VM_Object_Table;

typedef struct Sce_VM_Symbol_Table {
	Sce_VM_Label_Table_Context global_filed;
	Sce_VM_Label_Table* label_table_;
	Sce_VM_Label_Table_Context* label_context;
	uint8_t * module_name_;
	uint32_t module_counter_;
	uint32_t module_counter_range_;
	const Sce_Virtual_Stack* sv_stack_point_;
	struct Sce_VM_Symbol_Table* next_;
}Sce_VM_Symbol_Table;

typedef struct Sce_Run_VM_Error_Status {
	uint8_t* error_str;
	uint8_t* error_func;
	uint8_t error_vm_fallback_code;
}Sce_Run_VM_Error_Status;
typedef int ctx_error_t;
typedef struct Sce_Run_VM_Context {
	//Sce_Run_VM_Tables root_symbols;
	Sce_VM_Symbol_Table root_symbol;
	Sce_Virutal_Registers svrs_;
	
	Sce_VM_Symbol_Table* vm_symbol_;
	
	Sce_VM_Symbol_Table* now_vm_symbol_;

	Sce_Binary_Machine_Instructions* inst_;
	Sce_Run_VM_Error_Status vm_error_status;
}Sce_Run_VM_Context;

#define GET_VM_CONTEXT_INST_NAME(inst_) (inst_).scope2

sce_si_strap Sce_Virutal_Register * get_svr(Sce_Run_VM_Context* sce_vm_context, uint32_t idx) {
	if (idx > SCE_VIRTUAL_REGISTER_MAX + 2)return NULL;
	return &sce_vm_context->svrs_.svr[idx];
}
sce_si_strap void delete_sce_run_extends_object(Sce_Ext_Values* ext_value) {
	if (!ext_value)return;
	switch (ext_value->ext_type){
	case E_Sce_Run_Ext_FILE_Value:
		fclose((FILE*)ext_value->ptr__);
	case E_Sce_Run_Ext_Null_Value:
	default:
		break;
	}
}
sce_si_strap void delete_sce_run_virtual_memory(Sce_Virtual_Memory* mem) {
	if (mem->is_const_value) return;
	else {
		switch (mem->sce_run_value.value_type){
		case E_Sce_Run_Mem_Value:
			//delete_sce_run_virtual_memory(mem->sce_run_value.mvalue);
			break;
		case E_Sce_Run_Obj_Value:
			delete_sce_run_extends_object(mem->sce_run_value.evalue);
			s_free(mem->sce_run_value.evalue);
			break;
		case E_Sce_Run_String_Value:
			delete_u8_string_buffers(mem->sce_run_value.ssvalue);
			s_free(mem->sce_run_value.ssvalue);
			break;

		default:
			break;
		}
		mem->is_const_value = IS_CONST_MEMORY;
		memset(&mem->sce_run_value, 0, sizeof(Sce_Run_Values));
	}
}
sce_si_strap void delete_sce_run_obj_context(Sce_VM_Object_Table* obj_table) {
	while (obj_table) {

		Sce_VM_Object_Table* obj_ = obj_table;

		//s_free(obj_->obj_name_);
		delete_sce_run_virtual_memory(&obj_->svm);
		obj_table = obj_table->next_;
		s_free(obj_);
	}
}
sce_si_strap void delete_sce_run_label_table(Sce_VM_Label_Table* ctx) {
	while (ctx) {
		Sce_VM_Label_Table* ctx_ = ctx;
		ctx = ctx->next_;
		s_free(ctx_);

	}
}
sce_si_strap void delete_sce_run_label_context(Sce_VM_Label_Table_Context* ctx) {
	while (ctx) {

		ctx->lebel_adress_ = NULL;
		Sce_VM_Label_Table_Context* ctx_ = ctx;
		delete_sce_run_obj_context(ctx_->obj_table_);
		ctx = ctx->next_;
		s_free(ctx_);

	}
}

sce_si_strap void delete_sce_run_global_filed_label_context(Sce_VM_Label_Table_Context* ctx) {
	while (ctx) {
		ctx->lebel_adress_ = NULL;
		Sce_VM_Label_Table_Context* ctx_ = ctx;

		delete_sce_run_obj_context(ctx_->obj_table_);

		ctx = ctx->next_;

	}
}
sce_si_strap void delete_sce_run_symbol_table(Sce_VM_Symbol_Table* ctx) {
	while (ctx) {
		s_free(ctx->module_name_);
		delete_sce_run_global_filed_label_context(&ctx->global_filed);
		delete_sce_run_label_context(ctx->label_context);
		delete_sce_run_label_table(ctx->label_table_);

		Sce_VM_Symbol_Table* ctx_ = ctx;
		ctx = ctx->next_;
		s_free(ctx_);
	}
}
sce_si_strap void delete_sce_run_root_symbol_table(Sce_VM_Symbol_Table* ctx) {
	while (ctx) {
		s_free(ctx->module_name_);
		delete_sce_run_global_filed_label_context(&ctx->global_filed);
		delete_sce_run_label_context(ctx->label_context);
		delete_sce_run_label_table(ctx->label_table_);
		ctx = ctx->next_;
	}
}
sce_si_strap void delete_sce_run_vm_context(Sce_Run_VM_Context*ctx) {
	//delete_sce_run_root_symbol_table(&ctx->root_symbol);
	delete_sce_run_symbol_table(ctx->vm_symbol_);
	s_free(ctx->vm_error_status.error_str);
	s_free(ctx->vm_error_status.error_func);
	memset(ctx, 0, sizeof(Sce_Run_VM_Context));
}
//Sce_Binary_Machine_Instructions* set_start_vm_context(Sce_Run_VM_Context* sce_vm_context, const uint8_t* name_);
Sce_Run_VM_Context start_sce_vm_context(Sce_Binary_Machine_Instructions* sce_vm_inst);
void run_collect_label_sce_vm_context(Sce_Run_VM_Context* sce_vm_context);
void run_sce_vm_context(Sce_Run_VM_Context* sce_vm_context, const uint8_t* name_);

void system_call_sce_vm(Sce_Run_VM_Context* vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void alloc_sce_vm_context_local(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void alloc_sce_vm_context(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void add_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void smov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void bmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void cmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void fmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void imov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void rmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void mmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void mov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst);
void set_lable_name_sce_vm(Sce_Run_VM_Context* sce_vm_context);
void set_module_name_sce_vm(Sce_Run_VM_Context* sce_vm_context);

#endif // !_SCE_RUN_H_
