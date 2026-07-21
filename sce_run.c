#include "sce_run.h"
#include "sce_system_call.h"
#define GET_ERROR_STRING(dst, format, ...)\
	do{\
		uint32_t req = snprintf(NULL, 0, "error : "format, __VA_ARGS__);\
		req += 1;\
		dst =\
		smart_malloc(uint8_t, req);\
		if (!dst) break;\
		(void)snprintf(dst, req, "error : "format, __VA_ARGS__); \
	} while (0)

#define get_vm_error_status(dst, format, ...)\
	do{\
		GET_ERROR_STRING((dst).vm_error_status.error_str, format, __VA_ARGS__);\
		if(!(dst).vm_error_status.error_str) (dst).vm_error_status.error_vm_fallback_code = SCE_VM_FALLBACK_CODE;\
	}while(0)


#define operator_sce_function_vm(ope, opestr) \
	if (mem1->sce_run_value.value_type == E_Sce_Run_Mem_Value) { \
		mem1 = mem1->sce_run_value.mvalue; \
		if (mem2->sce_run_value.value_type == E_Sce_Run_Mem_Value) { \
			mem2 = mem2->sce_run_value.mvalue; \
		} \
	} \
	else { \
		if (mem2->sce_run_value.value_type == E_Sce_Run_Mem_Value) { \
			mem2 = mem2->sce_run_value.mvalue; \
		} \
	} \
	if (mem1->sce_run_value.value_type == E_Sce_Run_Mem_Value || mem2->sce_run_value.value_type == E_Sce_Run_Mem_Value) { \
		GET_ERROR_STRING( \
			error_status->error_str, \
			"Reference objects cannot be " opestr " together." \
		); \
		if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE; \
		return; \
	} \
	bool d_ = mem1->sce_run_value.value_type == mem2->sce_run_value.value_type; \
	if (d_) { \
		switch (mem1->sce_run_value.value_type) { \
		case E_Sce_Run_String_Value: \
			GET_ERROR_STRING( \
				error_status->error_str, \
				opestr " operations between string and string are not supported." \
			); \
			if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE; \
			break; \
		case E_Sce_Run_Int_Value: \
			mem1->sce_run_value.ivalue ope##= mem2->sce_run_value.ivalue; \
			break; \
		case E_Sce_Run_Bool_Value: \
			mem1->sce_run_value.ivalue = (uint32_t)mem1->sce_run_value.bvalue ope (uint32_t)mem2->sce_run_value.bvalue; \
			mem1->sce_run_value.value_type = E_Sce_Run_Int_Value; \
			break; \
		case E_Sce_Run_Float_Value: \
			mem1->sce_run_value.fvalue ope##= mem2->sce_run_value.fvalue; \
			break; \
		case E_Sce_Run_Double_Value: \
			mem1->sce_run_value.dvalue ope##= mem2->sce_run_value.dvalue; \
			break; \
		case E_Sce_Run_Null_Value: \
			break; \
		case E_Sce_Run_Virtual_Value: \
			break; \
		case E_Sce_Run_Obj_Value: \
			GET_ERROR_STRING( \
				error_status->error_str, \
				opestr " using objects is not possible" \
			); \
			break; \
		case E_Sce_Run_Char_Value: \
		{ \
			mem1->sce_run_value.ivalue = (uint32_t)mem1->sce_run_value.cvalue ope (uint32_t)mem2->sce_run_value.cvalue; \
			mem1->sce_run_value.value_type = E_Sce_Run_Int_Value; \
		} \
			break; \
		} \
		return; \
	} \
	d_ = mem1->sce_run_value.value_type > mem2->sce_run_value.value_type; \
	if (d_) { \
		switch (mem1->sce_run_value.value_type) { \
		case E_Sce_Run_Obj_Value: \
			GET_ERROR_STRING( \
				error_status->error_str, \
				opestr " using objects is not possible" \
			); \
			break; \
		case E_Sce_Run_String_Value: \
			switch (mem2->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
				append_u8_byte_u8_string( \
					mem1->sce_run_value.ssvalue, \
					(uint8_t*)"null" \
				); \
				break; \
			case E_Sce_Run_Virtual_Value: \
				append_u8_byte_u8_string( \
					mem1->sce_run_value.ssvalue, \
					(uint8_t*)"virtual" \
				); \
				break; \
			default: \
				GET_ERROR_STRING( \
					error_status->error_str, \
					opestr " operations between %s and %s are not supported.", \
					run_value_type_str[mem1->sce_run_value.value_type], \
					run_value_type_str[mem2->sce_run_value.value_type] \
				); \
				if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE; \
				break; \
			} \
			break; \
		case E_Sce_Run_Int_Value: \
			switch (mem2->sce_run_value.value_type) { \
			case E_Sce_Run_Virtual_Value: break; \
			case E_Sce_Run_Null_Value: break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.ivalue ope##= (uint32_t)mem2->sce_run_value.bvalue; \
				break; \
			case E_Sce_Run_Char_Value: \
				mem1->sce_run_value.ivalue ope##= (uint32_t)mem2->sce_run_value.cvalue; \
				break; \
			} \
			break; \
		case E_Sce_Run_Float_Value: \
			switch (mem2->sce_run_value.value_type) { \
			case E_Sce_Run_Virtual_Value: break; \
			case E_Sce_Run_Null_Value: break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.fvalue ope##= (float)mem2->sce_run_value.bvalue; \
				break; \
			case E_Sce_Run_Char_Value: \
				mem1->sce_run_value.fvalue ope##= (float)mem2->sce_run_value.cvalue; \
				break; \
			case E_Sce_Run_Int_Value: \
				mem1->sce_run_value.fvalue ope##= (float)mem2->sce_run_value.ivalue; \
				break; \
			} \
			break; \
		case E_Sce_Run_Double_Value: \
			switch (mem2->sce_run_value.value_type) { \
			case E_Sce_Run_Virtual_Value: break; \
			case E_Sce_Run_Null_Value: break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.dvalue ope##= (double)mem2->sce_run_value.bvalue; \
				break; \
			case E_Sce_Run_Char_Value: \
				mem1->sce_run_value.dvalue ope##= (double)mem2->sce_run_value.cvalue; \
				break; \
			case E_Sce_Run_Int_Value: \
				mem1->sce_run_value.dvalue ope##= (double)mem2->sce_run_value.ivalue; \
				break; \
			case E_Sce_Run_Float_Value: \
				mem1->sce_run_value.dvalue ope##= (double)mem2->sce_run_value.fvalue; \
				break; \
			} \
			break; \
		} \
	} \
	else { \
		switch (mem2->sce_run_value.value_type) { \
		case E_Sce_Run_Obj_Value: \
			GET_ERROR_STRING( \
				error_status->error_str, \
				opestr " using objects is not possible" \
			); \
			if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE; \
			break; \
		case E_Sce_Run_String_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
				break; \
			case E_Sce_Run_Virtual_Value: \
				break; \
			default: \
				GET_ERROR_STRING( \
					error_status->error_str, \
					opestr " operations between %s and %s are not supported.", \
					run_value_type_str[mem1->sce_run_value.value_type], \
					run_value_type_str[mem2->sce_run_value.value_type] \
				); \
				if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE; \
				break; \
			} \
			break; \
		case E_Sce_Run_Double_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
			case E_Sce_Run_Virtual_Value: \
				mem1->sce_run_value.dvalue = mem2->sce_run_value.dvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value; \
				break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.dvalue = (double)mem1->sce_run_value.bvalue ope (double)mem2->sce_run_value.dvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value; \
				break; \
			case E_Sce_Run_Char_Value: \
				mem1->sce_run_value.dvalue = (double)mem1->sce_run_value.cvalue ope (double)mem2->sce_run_value.dvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value; \
				break; \
			case E_Sce_Run_Int_Value: \
				mem1->sce_run_value.dvalue = (double)mem1->sce_run_value.ivalue ope (double)mem2->sce_run_value.dvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value; \
				break; \
			case E_Sce_Run_Float_Value: \
				mem1->sce_run_value.dvalue = (double)mem1->sce_run_value.fvalue ope (double)mem2->sce_run_value.dvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value; \
				break; \
			} \
			break; \
		case E_Sce_Run_Float_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
			case E_Sce_Run_Virtual_Value: \
				mem1->sce_run_value.fvalue = mem2->sce_run_value.fvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value; \
				break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.fvalue = (float)mem1->sce_run_value.bvalue ope (float)mem2->sce_run_value.fvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value; \
				break; \
			case E_Sce_Run_Char_Value: \
				mem1->sce_run_value.fvalue = (float)mem1->sce_run_value.cvalue ope (float)mem2->sce_run_value.fvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value; \
				break; \
			case E_Sce_Run_Int_Value: \
				mem1->sce_run_value.fvalue = (float)mem1->sce_run_value.ivalue ope (float)mem2->sce_run_value.fvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value; \
				break; \
			} \
			break; \
		case E_Sce_Run_Int_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
			case E_Sce_Run_Virtual_Value: \
				mem1->sce_run_value.ivalue = mem2->sce_run_value.ivalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value; \
				break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.ivalue = (uint32_t)mem1->sce_run_value.bvalue ope (uint32_t)mem2->sce_run_value.ivalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value; \
				break; \
			case E_Sce_Run_Char_Value: \
				mem1->sce_run_value.ivalue = (uint32_t)mem1->sce_run_value.cvalue ope (uint32_t)mem2->sce_run_value.ivalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value; \
				break; \
			} \
			break; \
		case E_Sce_Run_Char_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
			case E_Sce_Run_Virtual_Value: \
				mem1->sce_run_value.cvalue = mem2->sce_run_value.cvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Char_Value; \
				break; \
			case E_Sce_Run_Bool_Value: \
				mem1->sce_run_value.ivalue = (uint32_t)mem1->sce_run_value.bvalue ope (uint32_t)mem2->sce_run_value.cvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value; \
				break; \
			} \
			break; \
		case E_Sce_Run_Bool_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
			case E_Sce_Run_Virtual_Value: \
				mem1->sce_run_value.bvalue = mem2->sce_run_value.bvalue; \
				mem1->sce_run_value.value_type = E_Sce_Run_Bool_Value; \
				break; \
			} \
			break; \
		case E_Sce_Run_Virtual_Value: \
			switch (mem1->sce_run_value.value_type) { \
			case E_Sce_Run_Null_Value: \
				mem1->sce_run_value.value_type = E_Sce_Run_Virtual_Value; \
				break; \
			} \
			break; \
		case E_Sce_Run_Null_Value: \
			break; \
		} \
	} 
#define check_global_module_name(str1) simple_strcmp(str1, global_label)
#define get_global_module_symbol_table(vm_ctx) &((vm_ctx).root_symbol)
#define access_vm_ctx_svr(vm_ctx, svr_idx) (vm_ctx).svrs_.svr[svr_idx]
#define eqaul_now_ctx_global(ctx_) &(ctx_).now_vm_symbol_->global_filed == (ctx_).now_vm_symbol_->label_context

#define is_svr_type(srv_ptr, type) srv_ptr->sce_run_value.value_type == type
#define access_svr_mvalue(svr) ((svr).sce_run_value.mvalue)
#define is_svr_value_type(svr, type) (svr).sce_run_value.value_type == type
#define get_svr_ivalue(svr) (svr).sce_run_value.ivalue
#define GLOBAL_LABEL (0x1 << 0x0)
#define LOCAL_LABEL (0x1 << 0x1)
#define INIT_FINISHED   (0x1 << 0x2)

#define SCE_SYSTEM_STDOUT 0
#define SCE_SYSTEM_STDIN  1
#define SCE_SYSTEM_STDERR 2

static uint8_t* get_svr_value_to_str_(bool* is_ref, uint32_t* size_, Sce_Virutal_Register* svr) {
	if (is_svr_value_type(*svr, E_Sce_Run_String_Value)) {
		uint8_t* dst_ = svr->sce_run_value.ssvalue->str__;
		*size_ = (uint32_t)strlen(dst_);
		*is_ref = true;
		return dst_;
	}
	else if (is_svr_value_type(*svr, E_Sce_Run_Obj_Value)) return NULL;
	else if (is_svr_value_type(*svr, E_Sce_Run_Mem_Value)) {  return NULL; }
	else if (is_svr_value_type(*svr, E_Sce_Run_Int_Value)) {

		return to_32_str(svr->sce_run_value.ivalue, size_);
	}
	else if (is_svr_value_type(*svr, E_Sce_Run_Float_Value)) {
		return to_float_str(svr->sce_run_value.fvalue, size_);
	}

	else if (is_svr_value_type(*svr, E_Sce_Run_Double_Value)) {
		return to_double_str(svr->sce_run_value.dvalue, size_);
	}
	return NULL;
}
static uint8_t* get_svr_value_to_str(bool* is_ref, uint32_t* size_, Sce_Virutal_Register* svr) {
	if (is_svr_value_type(*svr, E_Sce_Run_String_Value)) {
		uint8_t* dst_ = svr->sce_run_value.ssvalue->str__;
		*size_ = u8strlen_u32(dst_);
		*is_ref = true;

		return dst_;
	}
	else if (is_svr_value_type(*svr, E_Sce_Run_Obj_Value)) return NULL;
	else if (is_svr_value_type(*svr, E_Sce_Run_Mem_Value)) return get_svr_value_to_str_(is_ref, size_, svr->sce_run_value.mvalue);
	else if (is_svr_value_type(*svr, E_Sce_Run_Int_Value)) {
		return to_32_str(svr->sce_run_value.ivalue, size_);
	}
	else if (is_svr_value_type(*svr, E_Sce_Run_Float_Value)) {
		return to_float_str(svr->sce_run_value.fvalue, size_);
	}

	else if (is_svr_value_type(*svr, E_Sce_Run_Double_Value)) {
		return to_double_str(svr->sce_run_value.dvalue, size_);
	}
	return NULL;
}
static FILE* sce_open_system_call(Sce_Virutal_Register* src_svr, int* open_check, bool is_mem_value) {
	if (is_svr_value_type(*src_svr, E_Sce_Run_String_Value)) {

	}
	else if (E_Sce_Run_Int_Value) {
		switch (src_svr->sce_run_value.ivalue) {
		case SCE_SYSTEM_STDOUT:
			*open_check = 1;

			return stdout;
		case SCE_SYSTEM_STDIN:
			*open_check = 1;

			return stdin;
		case SCE_SYSTEM_STDERR:
			*open_check = 1;

			return stderr;
		default:
			*open_check = -1;
			return NULL;

		}
	}
	else if (E_Sce_Run_Mem_Value) {
		if (!is_mem_value) sce_open_system_call(src_svr, open_check, true);
	}
	*open_check = -2;
	return NULL;
}

static FILE* get_svr_file_object_basic(Sce_Virutal_Register* svr, int* error_check) {
	if (svr->sce_run_value.value_type == E_Sce_Run_Obj_Value) {
		if (svr->sce_run_value.evalue->ext_type == E_Sce_Run_Ext_FILE_Value) {
			return (FILE*)svr->sce_run_value.evalue->ptr__;
		}
	}
	*error_check = -1;
	return NULL;
}
static FILE* get_svr_file_object(Sce_Virutal_Register* svr, int* error_check) {
	if (svr->sce_run_value.value_type == E_Sce_Run_Obj_Value) {
		if (svr->sce_run_value.evalue->ext_type == E_Sce_Run_Ext_FILE_Value) {
			return (FILE*)svr->sce_run_value.evalue->ptr__;
		}
	}
	else if (svr->sce_run_value.value_type == E_Sce_Run_Mem_Value) {
		return get_svr_file_object_basic(svr->sce_run_value.mvalue, error_check);
	}
	*error_check = -1;
	return NULL;
}
static void set_vm_context_svr_file(Sce_Run_VM_Context* vm_context, bool is_const, FILE* fp) {

	access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).is_const_value = is_const;
	access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.value_type = E_Sce_Run_Obj_Value;
	access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.evalue = smart_malloc(Sce_Ext_Values, 1);
	if (!access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.evalue) return;
	access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.evalue->ext_type = E_Sce_Run_Ext_FILE_Value;
	access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.evalue->ptr__ = (void*)fp;

}
static Sce_VM_Symbol_Table* search_module_name(Sce_VM_Symbol_Table * vm_modules, const uint8_t * name_) {
	while (vm_modules) {

		if (simple_strcmp(vm_modules->module_name_, name_)) return vm_modules;
		vm_modules = vm_modules->next_;
	}
	return NULL;
}

static Sce_VM_Label_Table* search_label_name(Sce_VM_Label_Table * vm_label, const uint8_t * name_) {
	while (vm_label) {


		if (simple_strcmp(vm_label->lebel_adress_->scope2, name_)) return vm_label;
		vm_label = vm_label->next_;
	}
	return NULL;
}

static void set_global_lable_name_sce_vm(Sce_Run_VM_Context* sce_vm_context) {
	Sce_VM_Label_Table* vm_label = search_label_name(
		sce_vm_context->root_symbol.label_table_, sce_vm_context->inst_->scope2
	);
	if (!vm_label) {
		Sce_VM_Label_Table* vm_label = smart_malloc(Sce_VM_Label_Table, 1);
		if (!vm_label) return;
		vm_label->lebel_adress_ = sce_vm_context->inst_;
		vm_label->next_ = sce_vm_context->root_symbol.label_table_;
		sce_vm_context->root_symbol.label_table_ = vm_label;
	}
	else {
		get_vm_error_status(
			*sce_vm_context,
			"Label %s redefined.",
			sce_vm_context->inst_->scope2
		);
	}
}
static Sce_VM_Symbol_Table* get_module_symbol_table(Sce_Run_VM_Context* sce_vm_context, const uint8_t* name_) {

	Sce_VM_Symbol_Table* module_;
	if (
		sce_vm_context->now_vm_symbol_ &&
		sce_vm_context->now_vm_symbol_->module_name_ &&
		simple_strcmp(sce_vm_context->now_vm_symbol_->module_name_, name_)
		) {
		module_ = sce_vm_context->now_vm_symbol_;
	}
	else {

		module_ = search_module_name(
			sce_vm_context->vm_symbol_, name_
		);
	}
	return module_;
}

static void run_vm_context_symbol(Sce_Run_VM_Context* vm_context) {
	Sce_VM_Label_Table_Context* label_table_ctx_ = vm_context->now_vm_symbol_->label_context;
	if (!label_table_ctx_) return;
	if (label_table_ctx_ == &vm_context->now_vm_symbol_->global_filed) return;
	Sce_VM_Label_Table_Context* next_ = label_table_ctx_->next_;
	label_table_ctx_->next_ = NULL;
	delete_sce_run_label_context(label_table_ctx_);
	/**
	* TODO:
	* ADD GC PROCESS TO LABEL CONTEXT.
	*/
	label_table_ctx_ = next_;
	vm_context->now_vm_symbol_->label_context = label_table_ctx_;
}
static Sce_VM_Label_Table* set_new_label_adress_vm_context(Sce_Run_VM_Context* vm_context, Sce_VM_Label_Table* label_) {
	/*
	Sce_VM_Label_Table* label_adress = smart_malloc(Sce_VM_Label_Table, 1);
	if (!label_adress) return NULL;
	run_vm_context_symbol(vm_context);
	label_adress->lebel_adress_ = label_->lebel_adress_;
	label_adress->next_ = NULL;
	vm_context->now_vm_symbol_->label_context->lebel_adress_ = label_adress;
	return label_;
	*/
	run_vm_context_symbol(vm_context);
	Sce_VM_Label_Table_Context * label_adress_ctx = smart_malloc(Sce_VM_Label_Table_Context, 1);
	if (!label_adress_ctx) return NULL;
	label_adress_ctx->lebel_adress_ = label_->lebel_adress_;
	label_adress_ctx->obj_table_ = NULL;
	label_adress_ctx->next_ = vm_context->now_vm_symbol_->label_context;
	vm_context->now_vm_symbol_->label_context = label_adress_ctx;
	return label_;
}
static void sce_save_register_to_tail_register(Sce_Run_VM_Context* vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	memcpy(&vm_context->svrs_.svr[sce_inst->ope2 + 1], &vm_context->svrs_.svr[1], sizeof(Sce_Virutal_Register) * sce_inst->ope1);

}

static void sce_load_register_to_tail_register(Sce_Run_VM_Context* vm_context, Sce_Binary_Machine_Instructions* sce_inst) {

	memcpy(&vm_context->svrs_.svr[1], &vm_context->svrs_.svr[sce_inst->ope1 + 1], sizeof(Sce_Virutal_Register) * sce_inst->ope2);
}
static Sce_Binary_Machine_Instructions* get_name_init_label_vm_context(Sce_Run_VM_Context* sce_vm_context) {
	return sce_vm_context->now_vm_symbol_->global_filed.lebel_adress_;
}
static void set_init_lable_name_isce_vm(Sce_Run_VM_Context* sce_vm_context) {
	sce_vm_context->vm_symbol_->global_filed.lebel_adress_ = sce_vm_context->inst_->next;
}

static void set_global_init_lable_name_isce_vm(Sce_Run_VM_Context* sce_vm_context) {
	sce_vm_context->root_symbol.global_filed.lebel_adress_ = sce_vm_context->inst_->next;
}
static Sce_Run_VM_Context gen_sce_vm_context() {
	return (Sce_Run_VM_Context) { 0 };
}

static int set_new_label_vm_context(Sce_Run_VM_Context* vm_context, Sce_Binary_Machine_Instructions** sce_inst) {
	Sce_VM_Label_Table* label_;
	label_ = search_label_name(vm_context->now_vm_symbol_->label_table_, (*sce_inst)->scope2);
	if (!label_) return -2;
	if (simple_strcmp((*sce_inst)->scope2, main_label)) {
		if (!set_new_label_adress_vm_context(vm_context, label_)) return -1;

		return 2;
	}
	else if (simple_strcmp((*sce_inst)->scope2, end_label)) {
		if (!set_new_label_adress_vm_context(vm_context, label_)) return -1;
		return 2;
	}
	else if (simple_strcmp((*sce_inst)->scope2, exit_label)) {
		run_vm_context_symbol(vm_context);
		return 2;
	}

	else {
		/*
		* CMP REQ
		*/
		Sce_Virutal_Register* svr128 = get_svr(vm_context, SCE_VIRTUAL_REGISTER_CMP_RETURN);
		if (svr128->sce_run_value.value_type == E_Sce_Run_Bool_Value) {
			if (svr128->sce_run_value.bvalue == false) return 3;
		}
		else return -3;
		(*sce_inst) = label_->lebel_adress_->next;
		return 1;
	}
}
static void run_collect_label_sce_vm_context(Sce_Run_VM_Context* sce_vm_context) {
	uint8_t is_label_mode = 0x0;
	while (sce_vm_context->inst_) {

		switch (sce_vm_context->inst_->sce_bmr_code) {
		case E_SCE_BINARY_INST_NAME__:
			if (check_global_module_name(sce_vm_context->inst_->scope2)) {
				is_label_mode = GLOBAL_LABEL;
				break;
			}
			is_label_mode = LOCAL_LABEL;
			set_module_name_sce_vm(
				sce_vm_context
			);

			break;
		case E_SCE_BINARY_INST_LABEL__:
			if (is_label_mode & GLOBAL_LABEL) {

				set_global_lable_name_sce_vm(
					sce_vm_context
				);
			}
			else if (is_label_mode & LOCAL_LABEL) {
				set_lable_name_sce_vm(
					sce_vm_context
				);
			}
			is_label_mode |= INIT_FINISHED;

			break;
		default:
			if (is_label_mode == GLOBAL_LABEL) {
				set_global_init_lable_name_isce_vm(sce_vm_context);

				is_label_mode |= INIT_FINISHED;
			}
			else if (is_label_mode == LOCAL_LABEL) {
				set_init_lable_name_isce_vm(sce_vm_context);

				is_label_mode |= INIT_FINISHED;
			}
		}
		sce_vm_context->inst_ = sce_vm_context->inst_->next;
	}

}

static void run_sce_vm_context_basic(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {

	while (sce_inst) {

		switch (
			sce_inst->sce_bmr_code
			) {
		case E_SCE_BINARY_INST_ADD__:
			add_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_JMP__: {
			int i;
			i = set_new_label_vm_context(sce_vm_context, &sce_inst);
			if (i == -2) {
				GET_ERROR_STRING(
					sce_vm_context->vm_error_status.error_str,
					"%s Lable not defined.",
					sce_inst->scope2
				);
			}
			else if (i == -3) {
				GET_ERROR_STRING(
					sce_vm_context->vm_error_status.error_str,
					"An invalid value is being used."
				);
			}
			else if (i == -1) {
				GET_ERROR_STRING(
					sce_vm_context->vm_error_status.error_str,
					"Memory alloc failed."
				);
			}
			else if (i == 1) {
				continue;
			}
			else if (i == 2) {
				return;
			}
			else if (i == 3) break;
			else assert(false);
		}
									break;
		case E_SCE_BINARY_INST_CALL__:
			/*TODO:*/
			break;
		case E_SCE_BINARY_INST_DELETE_REGISTER__:
			delete_sce_run_virtual_memory(&sce_vm_context->svrs_.svr[SCE_VIRTUAL_REGISTER_RETURN]);
			break;
		case E_SCE_BINARY_INST_SYSTEM_CALL__:

			system_call_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_SAVE_REGISTER__:
			//printf("%lu\n", sce_vm_context->svrs_.svr[1].sce_run_value.value_type);

			sce_save_register_to_tail_register(sce_vm_context, sce_inst);

			break;
		case E_SCE_BINARY_INST_LOAD_REGISTER__:

			sce_load_register_to_tail_register(sce_vm_context, sce_inst);

			break;
		case E_SCE_BINARY_INST_ALLOC__:
			alloc_sce_vm_context_local(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_MOV__:
			mov_sce_vm(sce_vm_context, sce_inst);

			break;
		case E_SCE_BINARY_INST_MMOV__:
			mmov_sce_vm(sce_vm_context, sce_inst);

			break;
		case E_SCE_BINARY_INST_RMOV__:
			rmov_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_BMOV__:
			bmov_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_CMOV__:
			cmov_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_IMOV__:
			imov_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_FMOV__:
			fmov_sce_vm(sce_vm_context, sce_inst);
			break;
		case E_SCE_BINARY_INST_SMOV__:
			smov_sce_vm(sce_vm_context, sce_inst);
			break;
		default:
			break;
		}
		if (sce_vm_context->vm_error_status.error_str) {
			printf("%s\n", sce_vm_context->vm_error_status.error_str);
			printf("%s\n", GET_SCE_IR_STRING(sce_inst));
			printf("Terminate the program.");
			break;
		}
		sce_inst = sce_inst->next;
	}
}
static Sce_Binary_Machine_Instructions* set_start_vm_context(Sce_Run_VM_Context* sce_vm_context, Sce_VM_Symbol_Table* module_) {


	if (!module_->label_table_) return NULL;
	if (module_->label_context) return module_->label_context->lebel_adress_;

	Sce_VM_Label_Table* label_ = search_label_name(module_->label_table_, start_label);
	if (!label_) return NULL;
	Sce_VM_Label_Table_Context* table_ctx = smart_malloc(Sce_VM_Label_Table_Context, 1);
	if (!table_ctx) return NULL;
	table_ctx->next_ = module_->label_context;
	table_ctx->obj_table_ = NULL;
	table_ctx->lebel_adress_ = label_->lebel_adress_;
	module_->label_context = table_ctx;
	sce_vm_context->now_vm_symbol_ = module_;
	return label_->lebel_adress_->next;
}

static void global_start_sce_vm_context(Sce_Run_VM_Context* sce_vm_context) {
	Sce_VM_Symbol_Table* global_module_ = get_global_module_symbol_table(*sce_vm_context);
	sce_vm_context->now_vm_symbol_ = global_module_;
	sce_vm_context->now_vm_symbol_->label_context = &sce_vm_context->now_vm_symbol_->global_filed;
	run_sce_vm_context_basic(sce_vm_context, global_module_->global_filed.lebel_adress_);
	sce_vm_context->now_vm_symbol_->global_filed.lebel_adress_ = NULL;
	sce_vm_context->now_vm_symbol_->label_context = NULL;
}
Sce_Run_VM_Context start_sce_vm_context(Sce_Binary_Machine_Instructions* sce_vm_inst) {
	Sce_Run_VM_Context sce_vm_context = gen_sce_vm_context();
	sce_vm_context.inst_ = sce_vm_inst;
	run_collect_label_sce_vm_context(&sce_vm_context);
	global_start_sce_vm_context(&sce_vm_context);
	return sce_vm_context;
}
void run_sce_vm_context(Sce_Run_VM_Context* sce_vm_context, const uint8_t* name_) {
	Sce_VM_Symbol_Table* module_ = get_module_symbol_table(sce_vm_context, name_);
	if (!module_) return;
	Sce_Binary_Machine_Instructions* sce_name_init_inst = module_->global_filed.lebel_adress_;

	if (sce_name_init_inst) {
		sce_vm_context->now_vm_symbol_ = module_;
		sce_vm_context->now_vm_symbol_->label_context = &module_->global_filed;

		run_sce_vm_context_basic(sce_vm_context, sce_name_init_inst);
		sce_vm_context->now_vm_symbol_->global_filed.lebel_adress_ = NULL;
		sce_vm_context->now_vm_symbol_->label_context = NULL;
	}

	Sce_Binary_Machine_Instructions* sce_inst = set_start_vm_context(sce_vm_context, module_);

	run_sce_vm_context_basic(sce_vm_context, sce_inst);

}
void run_sce_vm_global_context(Sce_Run_VM_Context* sce_vm_context) {
	sce_vm_context->root_symbol.module_name_;
}

void update_sce_vm_context(Sce_Run_VM_Context* sce_vm_inst, const uint8_t* name_) {

	run_sce_vm_context(sce_vm_inst, name_);

}
/*
* CREATE VM CONETEXT
* -->
* NAME ENTRY
* 
* 
* --->
* UPDATE
* START
* 
* UPDATE
* MAIN
* UPDATE
* END
*
*/
static void init_run_sce_vm_context_basic(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {

}
/*
Search for the start block.
Set the start block.
Then jump to the main block.
The default jump target is the main block.

start init
start now label
exit

Execution flow:
start init ->
start -> main -> end

start {
	// 'exit' has the same effect as jumping back to 'start'.
}

main {
	// Reaching the end of the main block has the same effect as jumping back to 'main'.
}

end {
	// 'exit' has the same effect as jumping back to 'start'.
}
*/
static int sce_putchar_call(FILE * file) {
	int i = fgetc(file);
	if (i == '\n') return 0;
	if (i == EOF) return 0;
	return i;
}
static void sce_read_system_call(Sce_Run_VM_Context* vm_context, Sce_Virutal_Register * svr1, FILE * in_fp) {
	svr1->is_const_value = NOT_IS_CONST_MEMORY;
	svr1->sce_run_value.value_type = E_Sce_Run_String_Value;
	svr1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
	if (!svr1->sce_run_value.ssvalue) return;
	init_u8_string_buffers(svr1->sce_run_value.ssvalue, 20);
	int c_;
	while (c_ = sce_putchar_call(in_fp)) {
		append_u8_byte_u8_string_buffer(svr1->sce_run_value.ssvalue, c_);
	}
}
void delete_sce_registers(Sce_Run_VM_Context* vm_context, uint32_t size_) {
	for (uint32_t i = 0; i < size_; ++i) {
		Sce_Virutal_Register* c_svr = &vm_context->svrs_.svr[i];
		delete_sce_run_virtual_memory(c_svr);
	}
}
void system_call_sce_vm(Sce_Run_VM_Context* vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	Sce_Virutal_Register* system_call_idx =
		get_svr(vm_context, 1);
	if (is_svr_value_type(*system_call_idx, E_Sce_Run_Int_Value)) {
		if (get_svr_ivalue(*system_call_idx) == SYSTEM_CALL_WRITE) {
			if (sce_inst->ope1 != system_call_write_args__) {
				get_vm_error_status(
					*vm_context,
					"System call 'write' expected %lu arguments, but got %lu.",
					system_call_write_args__,
					sce_inst->ope1
				);
				delete_sce_registers(vm_context, sce_inst->ope1);
				return;
			}
			Sce_Virutal_Register* args1 =
				get_svr(vm_context, 2);
			Sce_Virutal_Register* args2 =
				get_svr(vm_context, 3);

			uint32_t size_ = 0;
			bool is_ref = false;
			int check_ = 0;
			FILE * arg_file_obj = get_svr_file_object(args1, &check_);
			if (check_ < 0) {
				get_vm_error_status(
					*vm_context,
					"File Ptr %s is invalid object type.",
					run_value_type_str[args2->sce_run_value.value_type]
				);
				return;
			}
			else {
				if (!arg_file_obj) {
					get_vm_error_status(
						*vm_context,
						"FILE Object is NULL Ptr Object"
					);
					return;

				}
			}
			uint8_t* arg_buf = get_svr_value_to_str(&is_ref, &size_, args2);
			uint64_t res = 0;
			if (!arg_buf) {
				get_vm_error_status(
					*vm_context,
					"Out value %s is invalid object type.",
					run_value_type_str[args2->sce_run_value.value_type]
				);

			}
			else res = SYSTEM_CALL_WRITE__(arg_file_obj, arg_buf, size_);
			if (!is_ref) {
				s_free(arg_buf);
			}
			delete_sce_registers(vm_context, system_call_write_args__);
			access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).is_const_value = IS_CONST_MEMORY;
			access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.value_type = E_Sce_Run_Int_Value;
			access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN).sce_run_value.ivalue = res;
		}
		if (get_svr_ivalue(*system_call_idx) == SYSTEM_CALL_READ) {
			if (sce_inst->ope1 != system_call_read_args__) {
				get_vm_error_status(
					*vm_context,
					"System call 'read' expected %lu arguments, but got %lu.",
					system_call_read_args__,
					sce_inst->ope1
				);
				delete_sce_registers(vm_context, sce_inst->ope1);
				return;
			}
			Sce_Virutal_Register* args1 =
				get_svr(vm_context, 2);

			int check_ = 0;
			FILE* arg_file_obj = get_svr_file_object(args1, &check_);
			if (check_ < 0) {
				get_vm_error_status(
					*vm_context,
					"File Ptr %s is invalid object type.",
					run_value_type_str[args1->sce_run_value.value_type]
				);
				return;
			}
			else {
				if (!arg_file_obj) {
					get_vm_error_status(
						*vm_context,
						"FILE Object is NULL Ptr Object"
					);
					return;

				}
			}

			sce_read_system_call(vm_context, &access_vm_ctx_svr(*vm_context, SCE_VIRTUAL_REGISTER_RETURN), arg_file_obj);
			delete_sce_registers(vm_context, system_call_read_args__);

		}
		if (get_svr_ivalue(*system_call_idx) == SYSTEM_CALL_OPEN) {

			if (sce_inst->ope1 != system_call_open_args__) {
				get_vm_error_status(
					*vm_context,
					"System call 'open' expected %lu arguments, but got %lu.",
					system_call_open_args__,
					sce_inst->ope1
				);
				delete_sce_registers(vm_context, sce_inst->ope1);
				return;
			}

			Sce_Virutal_Register* args1 =
				get_svr(vm_context, 2);
			uint32_t size_ = 0;
			int open_check = 0;
			FILE* fp = sce_open_system_call(args1, &open_check, false);
			switch (open_check){
			case -1:

				get_vm_error_status(
					*vm_context,
					"No such %"PRIu64" file Value or directory",
					args1->sce_run_value.ivalue
				);
				break;
			case -2:

				get_vm_error_status(
					*vm_context,
					"Invalid argument type.",
				);
				break;
			case 1:
				set_vm_context_svr_file(vm_context, IS_CONST_MEMORY, fp);
				break;
			case 2:
				set_vm_context_svr_file(vm_context, NOT_IS_CONST_MEMORY, fp);
				break;
			default:
				break;
			}
			delete_sce_registers(vm_context, system_call_open_args__);

		}
	}
	else {
		get_vm_error_status(
			*vm_context,
			"Invalid object type."
		);
	}
}
void add_svr_basic_value(Sce_Run_VM_Error_Status* error_status, Sce_Virtual_Memory* mem1, Sce_Virutal_Register* mem2) {
	bool d_ = mem1->sce_run_value.value_type == mem2->sce_run_value.value_type;

}
void mul_svr_value(Sce_Run_VM_Error_Status* error_status, Sce_Virtual_Memory* mem1, Sce_Virutal_Register* mem2) {
	Sce_Virtual_Memory* tmp_mem2 = mem2;

	operator_sce_function_vm(
		*,
		"Multiplication"
	)
	delete_sce_run_virtual_memory(tmp_mem2);

}

void div_svr_value(Sce_Run_VM_Error_Status* error_status, Sce_Virtual_Memory* mem1, Sce_Virutal_Register* mem2) {
	Sce_Virtual_Memory* tmp_mem2 = mem2;

	operator_sce_function_vm(
		/ ,
		"Division"
	)
	delete_sce_run_virtual_memory(tmp_mem2);
}

void mod_svr_value(Sce_Run_VM_Error_Status* error_status, Sce_Virtual_Memory* mem1, Sce_Virutal_Register* mem2) {

}
void sub_svr_value(Sce_Run_VM_Error_Status* error_status, Sce_Virtual_Memory* mem1, Sce_Virutal_Register* mem2) {
	Sce_Virtual_Memory* tmp_mem2 = mem2;

	operator_sce_function_vm(
		-,
		"Subtraction"
	)
	delete_sce_run_virtual_memory(tmp_mem2);

}
void add_svr_value(Sce_Run_VM_Error_Status * error_status, Sce_Virtual_Memory* mem1, Sce_Virutal_Register* mem2) {
	Sce_Virtual_Memory* tmp_mem2 = mem2;
	if (mem1->sce_run_value.value_type == E_Sce_Run_Mem_Value) {
		mem1 = mem1->sce_run_value.mvalue;
		if (mem2->sce_run_value.value_type == E_Sce_Run_Mem_Value) {
			mem2 = mem2->sce_run_value.mvalue;
		}
	} 
	else {

		if (mem2->sce_run_value.value_type == E_Sce_Run_Mem_Value) {
			mem2 = mem2->sce_run_value.mvalue;

		}
	}
	if (mem1->sce_run_value.value_type == E_Sce_Run_Mem_Value || mem2->sce_run_value.value_type == E_Sce_Run_Mem_Value) {
		GET_ERROR_STRING(
			error_status->error_str,
			"Reference objects cannot be added together."
		);
		if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
		return;
	}
	bool d_ = mem1->sce_run_value.value_type == mem2->sce_run_value.value_type;
	/*
	*
	* 
	* 1NULL + 2NULL -> 1NULL
	* 
	* 1VIRTUAL + 2VIRTUAL -> 1VIRTUAL
	* 
	* 
	* 1 + 2 = 3
	* 1 + 1.0 -> 1.0
	* MOV R0 I
	* MOV R1 "HELLO"
	* ADD R0 R1
	* I + "HELLO"
	* 
	*/
	if (d_) {
		switch (mem1->sce_run_value.value_type) {
		case E_Sce_Run_String_Value:
			append_u8_string_buffer_u8_string_buffer(
				mem1->sce_run_value.ssvalue,
				mem2->sce_run_value.ssvalue
			);
			//delete_sce_run_virtual_memory(mem2);
			break;
		case E_Sce_Run_Int_Value:
			mem1->sce_run_value.ivalue += mem2->sce_run_value.ivalue;
			break;
		case E_Sce_Run_Bool_Value:
			mem1->sce_run_value.ivalue = (uint32_t)mem1->sce_run_value.bvalue + (uint32_t)mem2->sce_run_value.bvalue;
			mem1->sce_run_value.value_type = E_Sce_Run_Int_Value;
			break;
		case E_Sce_Run_Float_Value:
			mem1->sce_run_value.fvalue += mem2->sce_run_value.fvalue;
			break;
		case E_Sce_Run_Double_Value:
			mem1->sce_run_value.dvalue += mem2->sce_run_value.dvalue;
			break;
		case E_Sce_Run_Null_Value:

			break;
		case E_Sce_Run_Virtual_Value:

			break;
		case E_Sce_Run_Obj_Value:
			GET_ERROR_STRING(
				error_status->error_str,
				"Objects cannot be added together."
			);
			break;
		case E_Sce_Run_Char_Value:
			/*TODO:*/
		{
			uint8_t c_ = mem1->sce_run_value.cvalue;
			mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
			if (!mem1->sce_run_value.ssvalue) {
				error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
				break;
			}
			append_u8_byte_u8_string_buffer(mem1->sce_run_value.ssvalue, c_);
			append_u8_byte_u8_string_buffer(mem1->sce_run_value.ssvalue, mem2->sce_run_value.cvalue);
			mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
		}
			break;
		}
		goto END_ADD_PROCESS;
	}
	/**
	* 1         2
	* FLOAT > INT
	* YES
	* OBJ > INT
	* NO
	* 
	*/
	d_ = mem1->sce_run_value.value_type > mem2->sce_run_value.value_type;
	if (d_) {
		switch (mem1->sce_run_value.value_type) {
		case E_Sce_Run_Obj_Value:
			GET_ERROR_STRING(
				error_status->error_str,
				"Addition using objects is not possible"
			);

			break;

		case E_Sce_Run_String_Value:
			switch (mem2->sce_run_value.value_type) {
			case E_Sce_Run_Int_Value:
				to_u32_u8strbuf(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ivalue
				);
				break;
			case E_Sce_Run_Float_Value:
				to_float_u8strbuf(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.fvalue
				);
				break;
			case E_Sce_Run_Double_Value:
				to_double_u8strbuf(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.dvalue
				);
				break;
			case E_Sce_Run_Bool_Value:
				append_u8_byte_u8_string(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.bvalue ? (uint8_t*)"true" : (uint8_t *)"false"
				);
				break;
			case E_Sce_Run_Null_Value:
				append_u8_byte_u8_string(
					mem1->sce_run_value.ssvalue,
					(uint8_t *)"null"
				);
				break;
			case E_Sce_Run_Virtual_Value:
				append_u8_byte_u8_string(
					mem1->sce_run_value.ssvalue,
					(uint8_t*)"virtual"
				);
				break;
			case E_Sce_Run_Char_Value:
				append_u8_byte_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.cvalue
				);
				break;
			}
			break;
		case E_Sce_Run_Int_Value:
			switch (mem2->sce_run_value.value_type) {
			case E_Sce_Run_Virtual_Value:break;/*1 + Null = 1*/
			case E_Sce_Run_Null_Value:break;/*1 + Virtual = 1*/
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.ivalue += (uint32_t)mem2->sce_run_value.bvalue;
				break;
			case E_Sce_Run_Char_Value:
				mem1->sce_run_value.ivalue += (uint32_t)mem2->sce_run_value.cvalue;
				break;
			}
			break;

		case E_Sce_Run_Float_Value:
			switch (mem2->sce_run_value.value_type) {
			case E_Sce_Run_Virtual_Value:break;/*1 + Null = 1*/
			case E_Sce_Run_Null_Value:break;/*1 + Virtual = 1*/
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.fvalue += (float)mem2->sce_run_value.bvalue;
				break;
			case E_Sce_Run_Char_Value:
				mem1->sce_run_value.fvalue += (float)mem2->sce_run_value.cvalue;
				break;

			case E_Sce_Run_Int_Value:
				mem1->sce_run_value.fvalue += (float)mem2->sce_run_value.ivalue;
				break;
			}
			break;

		case E_Sce_Run_Double_Value:
			switch (mem2->sce_run_value.value_type) {
			case E_Sce_Run_Virtual_Value:break;/*1 + Null = 1*/
			case E_Sce_Run_Null_Value:break;/*1 + Virtual = 1*/
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.dvalue += (double)mem2->sce_run_value.bvalue;
				break;
			case E_Sce_Run_Char_Value:
				mem1->sce_run_value.dvalue += (double)mem2->sce_run_value.cvalue;
				break;

			case E_Sce_Run_Int_Value:
				mem1->sce_run_value.dvalue += (double)mem2->sce_run_value.ivalue;
				break;
			case E_Sce_Run_Float_Value:
				mem1->sce_run_value.dvalue += (double)mem2->sce_run_value.fvalue;
				break;
			}
			break;

		}
	}
	else {
		switch (mem2->sce_run_value.value_type) {
		case E_Sce_Run_Obj_Value:
			GET_ERROR_STRING(
				error_status->error_str,
				"Addition using objects is not possible"
			);
			if (!error_status->error_str) error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
			break;

		case E_Sce_Run_String_Value:
			mem1->is_const_value = NOT_IS_CONST_MEMORY;

			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				append_u8_byte_u8_string(
					mem1->sce_run_value.ssvalue,
					(uint8_t*)"null"
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;

			case E_Sce_Run_Virtual_Value:
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				append_u8_byte_u8_string(
					mem1->sce_run_value.ssvalue,
					(uint8_t*)"virtual"
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;

			case E_Sce_Run_Bool_Value: {
				bool b_ = mem1->sce_run_value.bvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				append_u8_byte_u8_string(
					mem1->sce_run_value.ssvalue,
					b_ ? (uint8_t*)"true" : (uint8_t*)"false"
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;
			}

			case E_Sce_Run_Char_Value: {
				uint8_t c_ = mem1->sce_run_value.cvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				insert_u8_byte_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					c_
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;
			}

			case E_Sce_Run_Int_Value: {
				uint64_t v_ = mem1->sce_run_value.ivalue;
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				to_u32_u8strbuf(
					mem1->sce_run_value.ssvalue,
					v_
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;
			}

			case E_Sce_Run_Float_Value: {
				float v_ = mem1->sce_run_value.fvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				to_float_u8strbuf(
					mem1->sce_run_value.ssvalue,
					v_
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;
			}

			case E_Sce_Run_Double_Value: {
				double v_ = mem1->sce_run_value.dvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_String_Value;
				mem1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
				if (!mem1->sce_run_value.ssvalue) {
					error_status->error_vm_fallback_code = SCE_VM_FALLBACK_CODE;
					return;
				}
				mem1->sce_run_value.ssvalue->index__ = 0;
				mem1->sce_run_value.ssvalue->size__ = 0;
				mem1->sce_run_value.ssvalue->str__ = NULL;
				to_double_u8strbuf(
					mem1->sce_run_value.ssvalue,
					v_
				);
				append_u8_string_buffer_u8_string_buffer(
					mem1->sce_run_value.ssvalue,
					mem2->sce_run_value.ssvalue
				);
				break;
			}
			}
			break;

		case E_Sce_Run_Double_Value:
			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
			case E_Sce_Run_Virtual_Value:
				mem1->sce_run_value.dvalue = mem2->sce_run_value.dvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value;
				break;
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.dvalue =
					(double)mem1->sce_run_value.bvalue +
					(double)mem2->sce_run_value.dvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value;
				break;
			case E_Sce_Run_Char_Value:
				mem1->sce_run_value.dvalue =
					(double)mem1->sce_run_value.cvalue +
					(double)mem2->sce_run_value.dvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value;
				break;
			case E_Sce_Run_Int_Value:
				mem1->sce_run_value.dvalue =
					(double)mem1->sce_run_value.ivalue +
					(double)mem2->sce_run_value.dvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value;
				break;
			case E_Sce_Run_Float_Value:
				mem1->sce_run_value.dvalue =
					(double)mem1->sce_run_value.fvalue +
					(double)mem2->sce_run_value.dvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Double_Value;
				break;
			}
			break;

		case E_Sce_Run_Float_Value:
			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
			case E_Sce_Run_Virtual_Value:
				mem1->sce_run_value.fvalue = mem2->sce_run_value.fvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value;
				break;
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.fvalue =
					(float)mem1->sce_run_value.bvalue +
					(float)mem2->sce_run_value.fvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value;
				break;
			case E_Sce_Run_Char_Value:
				mem1->sce_run_value.fvalue =
					(float)mem1->sce_run_value.cvalue +
					(float)mem2->sce_run_value.fvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value;
				break;
			case E_Sce_Run_Int_Value:
				mem1->sce_run_value.fvalue =
					(float)mem1->sce_run_value.ivalue +
					(float)mem2->sce_run_value.fvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Float_Value;
				break;
			}
			break;

		case E_Sce_Run_Int_Value:
			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
			case E_Sce_Run_Virtual_Value:
				mem1->sce_run_value.ivalue = mem2->sce_run_value.ivalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value;
				break;
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.ivalue =
					(uint32_t)mem1->sce_run_value.bvalue +
					(uint32_t)mem2->sce_run_value.ivalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value;
				break;
			case E_Sce_Run_Char_Value:
				mem1->sce_run_value.ivalue =
					(uint32_t)mem1->sce_run_value.cvalue +
					(uint32_t)mem2->sce_run_value.ivalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value;
				break;
			}
			break;

		case E_Sce_Run_Char_Value:
			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
			case E_Sce_Run_Virtual_Value:
				mem1->sce_run_value.cvalue = mem2->sce_run_value.cvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Char_Value;
				break;
			case E_Sce_Run_Bool_Value:
				mem1->sce_run_value.ivalue =
					(uint32_t)mem1->sce_run_value.bvalue +
					(uint32_t)mem2->sce_run_value.cvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Int_Value;
				break;
			}
			break;

		case E_Sce_Run_Bool_Value:
			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
			case E_Sce_Run_Virtual_Value:
				mem1->sce_run_value.bvalue = mem2->sce_run_value.bvalue;
				mem1->sce_run_value.value_type = E_Sce_Run_Bool_Value;
				break;
			}
			break;

		case E_Sce_Run_Virtual_Value:
			switch (mem1->sce_run_value.value_type) {
			case E_Sce_Run_Null_Value:
				mem1->sce_run_value.value_type = E_Sce_Run_Virtual_Value;
				break;
			}
			break;

		case E_Sce_Run_Null_Value:
			break;
		}
	}
	END_ADD_PROCESS:

	delete_sce_run_virtual_memory(tmp_mem2);
}

void add_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	Sce_Virutal_Register* svr2 =
		get_svr(sce_vm_context, sce_inst->ope2);
	add_svr_value(
		&sce_vm_context->vm_error_status,
		svr1, svr2
	);
}
static Sce_VM_Object_Table* search_object_symbol(Sce_VM_Object_Table* obj_table, const uint8_t * name_) {
	while (obj_table) {
		if (simple_strcmp(obj_table->obj_name_, name_)) {
			return obj_table;
		}
		obj_table = obj_table->next_;
	}
	return NULL;
}

static Sce_VM_Object_Table* search_local_object_symbol(Sce_VM_Label_Table_Context* local_context, const uint8_t* name_) {
	while (local_context) {
		search_object_symbol(
			local_context->obj_table_, name_
		);
		local_context = local_context->next_;
	}
	return NULL;
}
static Sce_VM_Object_Table * alloc_sce_vm_object_symbol(uint8_t* name_, Sce_VM_Object_Table* obj_table) {
	Sce_VM_Object_Table* new_ = smart_malloc(Sce_VM_Object_Table, 1);
	if (!new_) return obj_table;
	/*REFERENCE COPY*/
	new_->obj_name_ = name_;
	new_->svm.is_const_value = false;
	new_->svm.sce_run_value.value_type = E_Sce_Run_Null_Value;
	new_->next_ = NULL;
	new_->next_ = obj_table;
	return new_;
}

static void alloc_sce_vm_object_symbol_main(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	sce_vm_context->vm_symbol_->global_filed.obj_table_ = 
		alloc_sce_vm_object_symbol(sce_inst->scope2, sce_vm_context->now_vm_symbol_->global_filed.obj_table_);
}

static void alloc_sce_vm_object_symbol_local(Sce_VM_Label_Table_Context * sce_context, Sce_Binary_Machine_Instructions* sce_inst) {
	sce_context->obj_table_ = alloc_sce_vm_object_symbol(sce_inst->scope2, sce_context->obj_table_);
}
void alloc_sce_vm_context_local(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	
	uint8_t* symbol_ = GET_VM_CONTEXT_INST_NAME(*sce_inst);
	/*
	Sce_VM_Object_Table* obj_table_local_ = search_object_symbol(
		sce_vm_context->now_vm_symbol_->label_context->obj_table_, sce_inst->scope2);
	if (!obj_table_local_) {
		*/

	alloc_sce_vm_object_symbol_local(sce_vm_context->now_vm_symbol_->label_context, sce_inst);
		/*
		return;
	}

	get_vm_error_status(
		*sce_vm_context,
		"error : Variable %s has been redefined.",
		obj_table_local_->obj_name_
	);
	*/
}
void alloc_sce_vm_context(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	uint8_t* symbol_ = GET_VM_CONTEXT_INST_NAME(*sce_inst);
	Sce_VM_Object_Table* obj_table_root_ = search_object_symbol(
		sce_vm_context->root_symbol.global_filed.obj_table_, sce_inst->scope2);

	Sce_VM_Object_Table* obj_table_ = search_object_symbol(
		sce_vm_context->now_vm_symbol_->global_filed.obj_table_, sce_inst->scope2);
	if (obj_table_) {

		get_vm_error_status(
			*sce_vm_context,
			"error : Variable %s has been redefined.",
			obj_table_->obj_name_
		);
		return;
	}
	alloc_sce_vm_object_symbol_main(sce_vm_context, sce_inst);
}

void smov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions * sce_inst) {
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	uint8_t * sbuf = sce_inst->scope2;
	if (svr1) {
		/*

		svr1->is_const_value = IS_CONST_MEMORY;
		svr1->sce_run_value.value_type = E_Sce_Run_String_Value;
		svr1->sce_run_value.svalue = sbuf;
		*/

		svr1->is_const_value = NOT_IS_CONST_MEMORY;
		svr1->sce_run_value.value_type = E_Sce_Run_String_Value;
		svr1->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
		if (!svr1->sce_run_value.ssvalue) return;
		uint32_t len_ = u8strlen_u32(sbuf);
		svr1->sce_run_value.ssvalue->index__ = len_;
		svr1->sce_run_value.ssvalue->size__  = len_ + 1;
		svr1->sce_run_value.ssvalue->str__   = sbuf;

		svr1->sce_run_value.ssvalue->str__ = smart_malloc(uint8_t, len_ + 1);
		if (!svr1->sce_run_value.ssvalue->str__) return;

		svr1->sce_run_value.ssvalue->size__ = len_ + 1;
		svr1->sce_run_value.ssvalue->index__ = len_;
		memcpy(
			svr1->sce_run_value.ssvalue->str__,
			sce_inst->scope2,
			svr1->sce_run_value.ssvalue->size__
		);
		return;
	}
	get_vm_error_status(
		*sce_vm_context,
		"Sce Virutal Register %lu is %lu or greater.",
		sce_inst->ope1,
		SCE_VIRTUAL_REGISTER_MAX
	);
}

void bmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	bool bbuf = sce_inst->bcope2;
	if (svr1) {
		svr1->is_const_value = true;
		svr1->sce_run_value.value_type = E_Sce_Run_Bool_Value;
		svr1->sce_run_value.cvalue = bbuf;
		return;
	}
	get_vm_error_status(
		*sce_vm_context,
		"Sce Virutal Register %lu is %lu or greater.",
		sce_inst->ope1,
		SCE_VIRTUAL_REGISTER_MAX
	);
}
void cmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	uint8_t cbuf = sce_inst->ccope2;
	if (svr1) {
		svr1->is_const_value = true;
		svr1->sce_run_value.value_type = E_Sce_Run_Char_Value;
		svr1->sce_run_value.cvalue = cbuf;
		return;
	}
	get_vm_error_status(
		*sce_vm_context,
		"Sce Virutal Register %lu is %lu or greater.",
		sce_vm_context->inst_->ope1,
		SCE_VIRTUAL_REGISTER_MAX
	);
}
void fmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	float fbuf = sce_inst->fcope2;
	if (svr1) {
		svr1->is_const_value = true;
		svr1->sce_run_value.value_type = E_Sce_Run_Float_Value;
		svr1->sce_run_value.fvalue = fbuf;
		return;

	}
	get_vm_error_status(
		*sce_vm_context,
		"Sce Virutal Register %lu is %lu or greater.",
		sce_inst->ope1,
		SCE_VIRTUAL_REGISTER_MAX
	);
}
void imov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	uint32_t ibuf = sce_inst->icope2;
	if (svr1) {
		svr1->is_const_value = true;
		svr1->sce_run_value.value_type = E_Sce_Run_Int_Value;
		svr1->sce_run_value.ivalue = ibuf;
		return;

	}
	get_vm_error_status(
		*sce_vm_context,
		"Sce Virutal Register %lu is %lu or greater.",
		sce_inst->ope1,
		SCE_VIRTUAL_REGISTER_MAX
	);
}

static void rmov_sce_vm_basic(Sce_Virutal_Register * rv1, Sce_Virutal_Register * rv2) {

	/*
	* i = 1;
	*
	* IMOV SVR0, 0
	* MOV  SVR1, &i
	* RMOV SVR1, SVR0
	* i = k;
	* MOV   SVR0, &i
	* MMOV SVR1, k
	* RMOV SVR0, SVR1
	* 0 = 1
	* IMOV SVR0, 0
	* IMOV SVR1, 1
	* RMOV SVR0, SVR1
	*
	*/
	if (is_svr_type(rv1, E_Sce_Run_Mem_Value)) {
		delete_sce_run_virtual_memory(rv1->sce_run_value.mvalue);
		if (is_svr_type(rv2, E_Sce_Run_Mem_Value)) {

			/*
			* Since the memory is saved as is,
			* we just need to copy the contents as they are.
			* Conceptually,
			* RV1 contains &i
			* so
			* RV2 contains &u
			* In other words,
			* by copying (*i) = (*u),
			* immediate values remain unchanged
			* classes are copied by reference as they are.
			*
			*/
			access_svr_mvalue(*rv1)->sce_run_value
			= access_svr_mvalue(*rv2)->sce_run_value;
			access_svr_mvalue(*rv1)->is_const_value = IS_CONST_MEMORY;
		}
		else {
			*access_svr_mvalue(*rv1) = *rv2;

		}
	}
	else *(rv1) = *(rv2);
}
void rmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	uint8_t* symbol_ = GET_VM_CONTEXT_INST_NAME(*sce_inst);
	Sce_Virutal_Register* svr1 =
		get_svr(sce_vm_context, sce_inst->ope1);
	Sce_Virutal_Register* svr2 =
		get_svr(sce_vm_context, sce_inst->ope2);
	if (svr2 && svr1) {

		rmov_sce_vm_basic(svr1, svr2);
		return;
	}
	if (!svr1) {
		get_vm_error_status(
			*sce_vm_context,
			"Sce Virutal Register %lu is %lu or greater.",
			sce_inst->ope1,
			SCE_VIRTUAL_REGISTER_MAX
		);
		return;
	}
	get_vm_error_status(
		*sce_vm_context,
		"Sce Virutal Register %lu is %lu or greater.",
		sce_inst->ope2,
		SCE_VIRTUAL_REGISTER_MAX
	);

}
static void mmov_mem_to_svr(Sce_Virutal_Register* svr, Sce_Virtual_Memory * mem) {
	/**
	* MEMORY 
	*/
	switch (mem->sce_run_value.value_type) {
	case E_Sce_Run_Mem_Value:
		/*SVR IS CONST MEMORY? OR NOT CONST MEMORY.*/
		svr->is_const_value = NOT_IS_CONST_MEMORY;
		svr->sce_run_value.mvalue = mem;
		svr->sce_run_value.value_type = E_Sce_Run_Mem_Value;
		break;
	case E_Sce_Run_String_Value:
		svr->is_const_value = NOT_IS_CONST_MEMORY;
		svr->sce_run_value.ssvalue = smart_malloc(U8_String_Buffers, 1);
		if (!svr->sce_run_value.ssvalue) return;
		init_u8_string_buffers(svr->sce_run_value.ssvalue, 20);
		svr->sce_run_value.value_type = mem->sce_run_value.value_type;
		append_u8_string_buffer_u8_string_buffer(svr->sce_run_value.ssvalue, mem->sce_run_value.ssvalue);
		break;
	default:
		svr->is_const_value = IS_CONST_MEMORY;
		memcpy(&svr->sce_run_value, &mem->sce_run_value, sizeof(Sce_Run_Values));
	}
}
void mmov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {
	uint8_t* symbol_ = GET_VM_CONTEXT_INST_NAME(*sce_inst);
	Sce_Virutal_Register* svr =
		get_svr(sce_vm_context, sce_inst->ope1);
	if (&sce_vm_context->root_symbol == sce_vm_context->now_vm_symbol_) goto Local_Search;

	Sce_VM_Object_Table* obj_table_root_ = search_object_symbol(sce_vm_context->root_symbol.global_filed.obj_table_, sce_inst->scope2);

	if (!svr) {

		get_vm_error_status(
			*sce_vm_context,
			"Sce Virutal Register %lu is %lu or greater.",
			sce_inst->ope1,
			SCE_VIRTUAL_REGISTER_MAX
		);

	}
	else {
		if (obj_table_root_) {
			mmov_mem_to_svr(svr, &obj_table_root_->svm);
		}
		else {
		Local_Search:
			{
				Sce_VM_Object_Table* obj_table_ = search_object_symbol(sce_vm_context->now_vm_symbol_->global_filed.obj_table_, sce_inst->scope2);
				if (obj_table_) {
					mmov_mem_to_svr(svr, &obj_table_->svm);
				}
				else {
					if (eqaul_now_ctx_global(*sce_vm_context)) return;
					obj_table_ = search_object_symbol(sce_vm_context->now_vm_symbol_->label_context->obj_table_, sce_inst->scope2);
					if (obj_table_) {
						mmov_mem_to_svr(svr, &obj_table_->svm);
						return;
					}
					get_vm_error_status(
						*sce_vm_context,
						"Variable %s not defined.",
						sce_inst->scope2
					);

				}
			}
		}
	}

}
void mov_sce_vm(Sce_Run_VM_Context* sce_vm_context, Sce_Binary_Machine_Instructions* sce_inst) {

	uint8_t* symbol_ = GET_VM_CONTEXT_INST_NAME(*sce_inst);
	Sce_Virutal_Register* svr =
		get_svr(sce_vm_context, sce_inst->ope1);
	if (&sce_vm_context->root_symbol == sce_vm_context->now_vm_symbol_) goto Local_Search;

	Sce_VM_Object_Table* obj_table_root_ = search_object_symbol(sce_vm_context->root_symbol.global_filed.obj_table_, sce_inst->scope2);

	if (!svr) {

		get_vm_error_status(
			*sce_vm_context,
			"Sce Virutal Register %lu is %lu or greater.",
			sce_inst->ope1,
			SCE_VIRTUAL_REGISTER_MAX
		);

	}
	else {
		if (obj_table_root_) {
			/**
			* MOV R0 &i
			* MOV R1 &j
			* RMOV R1 R0
			* R1.i.svalue = R0.j.svalue
			* R1.i.const = IS
			* 
			*/
			svr->is_const_value = NOT_IS_CONST_MEMORY;
			svr->sce_run_value.mvalue = &obj_table_root_->svm;
			svr->sce_run_value.value_type = E_Sce_Run_Mem_Value;
		}
		else {
		Local_Search:
			{
				Sce_VM_Object_Table* obj_table_ = search_object_symbol(sce_vm_context->now_vm_symbol_->global_filed.obj_table_, sce_inst->scope2);
				if (obj_table_) {
					svr->is_const_value = NOT_IS_CONST_MEMORY;
					svr->sce_run_value.mvalue = &obj_table_->svm;
					svr->sce_run_value.value_type = E_Sce_Run_Mem_Value;
				}
				else {

					if (eqaul_now_ctx_global(*sce_vm_context)) return;

					obj_table_ = search_object_symbol(sce_vm_context->now_vm_symbol_->label_context->obj_table_, sce_inst->scope2);
					if (obj_table_) {
						svr->is_const_value = NOT_IS_CONST_MEMORY;
						svr->sce_run_value.mvalue = &obj_table_->svm;
						svr->sce_run_value.value_type = E_Sce_Run_Mem_Value;
						return;
					}

					get_vm_error_status(
						*sce_vm_context,
						"Variable %s not defined.",
						sce_inst->scope2
					);

				}
			}
		}
	}

}

void set_lable_name_sce_vm(Sce_Run_VM_Context* sce_vm_context) {
	Sce_VM_Label_Table* vm_label = search_label_name(
		sce_vm_context->vm_symbol_->label_table_, sce_vm_context->inst_->scope2
	);
	if (!vm_label) {
		Sce_VM_Label_Table* vm_label = smart_malloc(Sce_VM_Label_Table, 1);
		if (!vm_label) return;
		vm_label->lebel_adress_ = sce_vm_context->inst_;
		vm_label->next_ = sce_vm_context->vm_symbol_->label_table_;
		sce_vm_context->vm_symbol_->label_table_ = vm_label;

	}
	else {
		get_vm_error_status(
			*sce_vm_context,
			"Label %s redefined.",
			sce_vm_context->inst_->scope2
		);
	}
}
void set_module_name_sce_vm(Sce_Run_VM_Context* sce_vm_context) {

	Sce_VM_Symbol_Table* vm_module = search_module_name(
		sce_vm_context->vm_symbol_, sce_vm_context->inst_->scope2
		);
	if (!vm_module) {
		Sce_VM_Symbol_Table* new_vm_module = smart_calloc(
			Sce_VM_Symbol_Table,
			1
		);

		if (!new_vm_module)return;
		new_vm_module->module_name_ = sce_vm_context->inst_->scope2;

		new_vm_module->next_ = sce_vm_context->vm_symbol_;
		sce_vm_context->vm_symbol_ = new_vm_module;
	}
	else {
		get_vm_error_status(
			*sce_vm_context,
			"Module %s redefined.",
			sce_vm_context->inst_->scope2
		);

	}
}
