#ifndef _SCE_IR_H_
#define _SCE_IR_H_
#include "sce_parser.h"
/*
NAME A:
LABEL END:
LABEL START;
GOTO %IF;
GOTO EXIT;
LABEL %IF:
GOTO EXIT;
GOTO EXIT

*/
#define SET_SCE_BINARY_MACHINE_TYPE(CONVERT_FUNC)\
CONVERT_FUNC(E_SCE_BINARY_INST_MOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_RMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_MMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_CMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_BMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_IMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_FMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_VMOV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_NAME__),\
CONVERT_FUNC(E_SCE_BINARY_INST_ADD__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SUB__),\
CONVERT_FUNC(E_SCE_BINARY_INST_MUL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_DIV__),\
CONVERT_FUNC(E_SCE_BINARY_INST_MOD__),\
CONVERT_FUNC(E_SCE_BINARY_INST_AND__),\
CONVERT_FUNC(E_SCE_BINARY_INST_OR__),\
CONVERT_FUNC(E_SCE_BINARY_INST_XOR__),\
CONVERT_FUNC(E_SCE_BINARY_INST_NOT__),\
CONVERT_FUNC(E_SCE_BINARY_INST_NOTL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SHR__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SHL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_EQ__),\
CONVERT_FUNC(E_SCE_BINARY_INST_NEQ__),\
CONVERT_FUNC(E_SCE_BINARY_INST_LS__),\
CONVERT_FUNC(E_SCE_BINARY_INST_GT__),\
CONVERT_FUNC(E_SCE_BINARY_INST_LSEQ__),\
CONVERT_FUNC(E_SCE_BINARY_INST_GTEQ__),\
CONVERT_FUNC(E_SCE_BINARY_INST_RET__),\
CONVERT_FUNC(E_SCE_BINARY_INST_CALL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_JMP__),\
CONVERT_FUNC(E_SCE_BINARY_INST_PHASE__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SYSTEM_CALL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_BUILT_IN_CALL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_LABEL__),\
CONVERT_FUNC(E_SCE_BINARY_INST_ALLOC__),\
CONVERT_FUNC(E_SCE_BINARY_INST_NAME_REPEAT__),\
CONVERT_FUNC(E_SCE_BINARY_INST_STACK_POINT__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SUB_STACK_POINT__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SAVE_REGISTER__),\
CONVERT_FUNC(E_SCE_BINARY_INST_LOAD_REGISTER__),\
CONVERT_FUNC(E_SCE_BINARY_INST_SAVE_REGISTER_STACK__),\
CONVERT_FUNC(E_SCE_BINARY_INST_LOAD_REGISTER_STACK__),\
CONVERT_FUNC(E_SCE_BINARY_INST_DELETE_REGISTER__),
#define E_SCE_IR_STSRING_OFFSET 17
#define GET_SCE_IR_STRING(ir) debug_sce_ir_strs[ir->sce_bmr_code] + E_SCE_IR_STSRING_OFFSET

static const uint8_t* debug_sce_ir_strs[] = {
	SET_SCE_BINARY_MACHINE_TYPE(CONVERT_DEFINE_STR)
};
static const char call_write_string__[] = "__write__";
static const char call_read_string__[]  = "__read__";
static const char call_open_string__[]  = "__open__";
static const char call_close_string__[] = "__close__";
static const char call_type_string__[]  = "type";
static const char call_len_string__[]   = "len";
static const char call_swap_string__[]   = "swap";
static const char call_sum_string__[]   = "sum";


static const char global_label[]    = "%global";
static const char start_label[]     = "%start";
static const char main_label[]      = "%main";
static const char end_label[]       = "%end";
static const char exit_label[]      = "%exit";
static const char if_label_prefix[] = "%%if%lu";
static const char left_hand_value_error[]        = "Error : The left-hand side value must be a substituted value.";
static const char already_name_value_error[]     = "Error : %s is already defined.";
static const char not_defined_name_value_error[] = "Error : %s is not defined.";
#define IF_LABEL_PREFIX_SIZE sizeof(if_label_prefix) - 1
#define START_LABEL_PREFIX_SIZE sizeof(start_label) - 1
#define MAIN_LABEL_PREFIX_SIZE sizeof(main_label) - 1
#define END_LABEL_PREFIX_SIZE sizeof(end_label) - 1
#define system_call_write_args__ 3
#define system_call_read_args__ 2
#define system_call_open_args__ 2
typedef enum Sce_Binary_Instruction {
	SET_SCE_BINARY_MACHINE_TYPE(CONVERT_DEFINE)
}Sce_Binary_Instruction;
#define SYSTEM_CALL_NULL    0x0
#define SYSTEM_CALL_WRITE   0x1
#define SYSTEM_CALL_READ    0x2
#define SYSTEM_CALL_OPEN    0x3
#define SYSTEM_CALL_CLOASE  0x4
#define SYSTEM_CALL_TYPE    0x5
#define SYSTEM_CALL_LEN     0x6
#define SYSTEM_CALL_WRITE_STRING call_write_string__
#define SYSTEM_CALL_READ_STRING  call_read_string__
#define SYSTEM_CALL_OPEN_STRING  call_open_string__
/**
* SVR IS SCE VIRTUAL REGISTER.
* THIS IS GENERAL PURPOSE OBJECTS TYPE REGISTER.
* 
* MOV SVR false
* MOV SVR vec3(1.0)
* MOV SVR 0
* MOV SVR SVR
* MOV SVR i
* 
* 
*/
#define CONST_128_BYTE (2u << 6u)
typedef struct Sce_Binary_Machine_Instrcution {
	struct Sce_Binary_Machine_Instructions* next;
	union Sce_Binarys {
		Sce_Binary_Instruction sce_bmr_code;
		uint32_t ope;
		int64_t iope;
		uint8_t* sope;
		void * vope;
		uint8_t cope;
		float fope;
		bool bope;
	} sce_b;

}Sce_Binary_Machine_Instrcution;
typedef struct Sce_Binary_Machine_Instructions {
	struct Sce_Binary_Machine_Instructions* next;
	Sce_Binary_Instruction sce_bmr_code;

	uint32_t ope1;
	union{
		uint32_t ope2;
		int64_t icope2;
		uint8_t ccope2;
		uint8_t* scope2;
		void* vcope2;
		float fcope2;
		double dcope2;
		bool bcope2;
	};
}Sce_Binary_Machine_Instructions;

typedef struct Object_Name_Chain {
	struct Object_Name_Chain* next_;
	const uint8_t* name_;
}Object_Name_Chain;
typedef struct Nest_Object_Chain {
	Object_Name_Chain* object_;
	struct Nest_Object_Chain* next_;
	const uint8_t* name_;
}Nest_Object_Chain;
typedef struct Sce_Create_Binary_Machine_Code_Data {
	Sce_Ast_Node* ast_;
	Sce_Binary_Machine_Instructions* sce_bmi;
	Sce_Binary_Machine_Instructions* sce_bmi_head;
	Sce_Binary_Machine_Instructions* sce_bmi_global;
	Sce_Binary_Machine_Instructions* sce_bmi_global_head;
	uint8_t has_phase_label;
	bool is_error;
	uint32_t sv_registers;
	uint32_t sv_local_label;
	uint8_t sv_local_label_buf[CONST_128_BYTE];
	uint32_t num;
	Nest_Object_Chain* now_;
	U8_String_Buffers now_nest_name_;
	const char* now_function_name_;
	size_t now_function_name_size_;

}Sce_Create_Binary_Machine_Code_Data;
/*
* 0 ERROR REGISTER
* 1 ~ 127
* RETURN
* 128
* 
*/

#define SCE_VIRTUAL_REGISTER_MAX 127
#define SCE_VIRTUAL_GENERAL_REGISTER_MAX 128
#define SCE_VIRTUAL_REGISTER_RETURN SCE_VIRTUAL_GENERAL_REGISTER_MAX
#define SCE_VIRTUAL_REGISTER_CMP_RETURN SCE_VIRTUAL_REGISTER_RETURN
#define SCE_VIRTUAL_REGISTER_MAX_ 129
#define SCE_VIRTUAL_STACK_MIN SCE_VIRTUAL_REGISTER_MAX_
#define GET_SCE_STACK_START_OFFSET(idx) idx - SCE_VIRTUAL_STACK_MIN

sce_si_strap uint32_t now_sce_virtual_register(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
	return sce_bm_data->sv_registers;
}
sce_si_strap uint32_t new_sce_virtual_register(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
	//if (sce_bm_data->sv_registers + 1 == SCE_VIRTUAL_REGISTER_CMP_RETURN) sce_bm_data->sv_registers++;
	return ++sce_bm_data->sv_registers;
}

sce_si_strap void delete_sce_virtual_register(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
	if (!sce_bm_data->sv_registers) return;
	--sce_bm_data->sv_registers;
}

sce_si_strap void delete_sce_virtual_registers(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, uint32_t idx) {
	if (sce_bm_data->sv_registers < idx) return;
	sce_bm_data->sv_registers -= idx;
}

sce_si_strap void reset_sce_virtual_register(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
	sce_bm_data->sv_registers = 0;
}

sce_si_strap void set_sce_virtual_register(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, uint32_t idx) {
	sce_bm_data->sv_registers = idx;
}
Sce_Binary_Machine_Instructions* sce_start_build_binary_run_code(bool * is_error, Sce_Ast_Node* left);
void build_sce_binary_run_code_module(Sce_Create_Binary_Machine_Code_Data* sce_bm_data);
void build_sce_binary_run_code_toplevel_statement(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
void build_sce_binary_run_code_name(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
void build_sce_binary_run_code_decl(Sce_Create_Binary_Machine_Code_Data* sce_bm_data);
void build_sce_binary_run_code_args(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
void build_sce_binary_run_code_sections(Sce_Binary_Machine_Instructions** init_inst_, Sce_Binary_Machine_Instructions** head_, Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_basic_symbol_iden_and_decl(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_basic_symbol_iden(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_basic_symbol_iden_and_decl(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_basic_const_iden(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_expr_statement(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_expr_ope(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
uint32_t build_sce_binary_run_code_basic_iden(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast);
#endif // !_SCE_IR_H_
