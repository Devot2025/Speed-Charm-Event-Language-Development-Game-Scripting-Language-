#include "sce_ir.h"
#include "sce_system_call.h"


#define HAS_START_LABEL (1 << 0)
#define HAS_MAIN_LABEL (1 << 1)
#define HAS_END_LABEL (1 << 2)

#define SET_SCE_HAS_PHASE_LABEL(src_sce_bm_data, has_label) src_sce_bm_data->has_phase_label |= has_label
#define SCE_HAS_PHASE_LABEL(src_sce_bm_data, has_label) (src_sce_bm_data->has_phase_label & has_label)
#define get_now_register(src_sce_bm_data) (src_sce_bm_data).sv_registers
static const uint8_t chain_fld_name[] = "";
#define CHAIN_FILED_NAMES chain_fld_name
#define is_register_return(reg_id) reg_id == SCE_VIRTUAL_REGISTER_RETURN
static Object_Name_Chain* new_object_name(Object_Name_Chain* now_, const uint8_t* name_) {
	Object_Name_Chain* new_ = smart_malloc(Object_Name_Chain, 1);
	if (!new_) return now_;
	new_->name_ = name_;
	new_->next_ = now_;
	return new_;
}
static bool set_new_object_name(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, const uint8_t* name_) {
	Object_Name_Chain* obj_ = sce_bm_data->now_->object_;
	while (obj_) {
		if (simple_strcmp(obj_->name_, name_)) return true;
		obj_ = obj_->next_;
	}

	sce_bm_data->now_->object_ = new_object_name(sce_bm_data->now_->object_, name_);
	return false;
}

static bool search_object_chain_name(Nest_Object_Chain* chain_, const uint8_t* name_) {
	Object_Name_Chain* obj_ = chain_->object_;
	while (obj_) {

		if (simple_strcmp(obj_->name_, name_)) {
			return true;
		}
		obj_ = obj_->next_;
	}
	return false;
}
static Nest_Object_Chain* search_object_name_in_nest_object_chain(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, const uint8_t* name_) {
	Nest_Object_Chain* chain_ = sce_bm_data->now_;
	while (chain_) {

		if (search_object_chain_name(chain_, name_)) return chain_;
		if (simple_strcmp(CHAIN_FILED_NAMES, chain_->name_)) {

		}
		chain_ = chain_->next_;
	}
	return NULL;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi(uint32_t ope1id, uint32_t ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->ope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_b(uint32_t ope1id, bool ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->bcope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_v(uint32_t ope1id, void* ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->vcope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_c(uint32_t ope1id, uint8_t ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->ccope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_f(uint32_t ope1id, float ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->fcope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_d(uint32_t ope1id, double ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->dcope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_i(uint32_t ope1id, int64_t ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->icope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_s(uint32_t ope1id, const uint8_t* ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->scope2 = dupalloc(ope2id, strlen(ope2id) + 1);
	dst_->sce_bmr_code = inst;
	return dst_;
}

static Sce_Binary_Machine_Instructions* gen_sce_bmi_mov_s(uint32_t ope1id, uint8_t* ope2id, Sce_Binary_Instruction inst) {
	Sce_Binary_Machine_Instructions* dst_ = smart_malloc(Sce_Binary_Machine_Instructions, 1);
	if (!dst_) return NULL;
	dst_->next = NULL;
	dst_->ope1 = ope1id;
	dst_->scope2 = ope2id;
	dst_->sce_bmr_code = inst;
	return dst_;
}

static void chain_sce_bmi_of_create_bm(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Binary_Machine_Instructions* sce_bmi) {
	if (!sce_bmi) return;
	if (!sce_bm_data->sce_bmi) {
		sce_bm_data->sce_bmi = sce_bmi;
		sce_bm_data->sce_bmi_head = sce_bmi;
	}
	else {
		sce_bm_data->sce_bmi->next = sce_bmi;
		sce_bm_data->sce_bmi = sce_bmi;
	}
}

static void chain_global_sce_bmi_of_create_bm(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Binary_Machine_Instructions* sce_bmi) {
	if (!sce_bmi) return;
	if (!sce_bm_data->sce_bmi_global) {
		sce_bm_data->sce_bmi_global = sce_bmi;
		sce_bm_data->sce_bmi_global_head = sce_bmi;

	}
	else {
		sce_bm_data->sce_bmi_global->next = sce_bmi;
		sce_bm_data->sce_bmi_global = sce_bmi;
	}
}
static void sce_virtual_register(uint32_t register_num) {
	if (register_num > SCE_VIRTUAL_REGISTER_MAX) return;
}
static uint32_t get_system_call_idx(Sce_Ast_Node* left) {
	if (!left) return 0;
	if (is_sce_ast_type(left, E_Sce_Ast_Iden)) {
		if (!left->data.buf) return 0;
		if (simple_strcmp(left->data.buf, SYSTEM_CALL_READ_STRING)) return SYSTEM_CALL_READ;
		if (simple_strcmp(left->data.buf, SYSTEM_CALL_WRITE_STRING)) return SYSTEM_CALL_WRITE;
		if (simple_strcmp(left->data.buf, SYSTEM_CALL_OPEN_STRING)) return SYSTEM_CALL_OPEN;
	}
	return 0;
}
static Sce_Binary_Machine_Instructions* delete_and_alalysis_binary_run_code(Sce_Binary_Machine_Instructions* begin_expr_bm) {
	if (begin_expr_bm) return NULL;
	while (begin_expr_bm) {
		switch (begin_expr_bm->sce_bmr_code){
		case E_SCE_BINARY_INST_SYSTEM_CALL__:
		case E_SCE_BINARY_INST_JMP__:
			break;
		default:
			break;
		}
		begin_expr_bm = begin_expr_bm->next;
	}
}

static Nest_Object_Chain* new_nest_object_chain(Nest_Object_Chain* now_nest, const uint8_t* name_) {
	Nest_Object_Chain* new_ = smart_malloc(Nest_Object_Chain, 1);
	if (!new_) return now_nest;
	new_->next_ = now_nest;
	new_->object_ = NULL;
	new_->name_ = name_;
	return new_;
}
static void set_new_nest_object_chain(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, const uint8_t* name_) {
	sce_bm_data->now_ = new_nest_object_chain(sce_bm_data->now_, name_);
}
static void delete_obj_name_chain(Object_Name_Chain* now_obj) {
	while (now_obj) {
		Object_Name_Chain* now_ = now_obj;
		now_obj = now_obj->next_;

		s_free(now_);
	}
}
static void delete_now_nest(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
	Nest_Object_Chain* now_ = sce_bm_data->now_;
	sce_bm_data->now_ = sce_bm_data->now_->next_;
	delete_obj_name_chain(now_->object_);
	s_free(now_);
}
static void save_now_sce_virtual_register(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {

}

static uint8_t* new_nest_symbol_name(uint8_t* name1_, const uint8_t* name2_) {
	size_t len1_ = safety_strlen(name1_);
	size_t len2_ = safety_strlen(name2_);
	uint8_t* new_ = smart_malloc(uint8_t, len1_ + len2_ + 1);
	if (!new_) return NULL;
	memcpy(new_, name1_, len1_);
	memcpy(new_ + len1_, name2_, len2_);
	new_[len1_ + len2_] = '\0';
	return new_;
}
static uint32_t build_sce_binary_run_code_call_function(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {

	uint32_t sys = get_system_call_idx(ast->left);
	/*
	* OBJ ADD(OBJ X, OBJ Y){}
	* MOV SVR0, 1
	* MOV SVR1, 2
	* CALL ADD
	* ALLOC X
	* ALLOC Y
	* MOV SVR2, X
	* MOV SVR3, Y
	* MOV SVR2, SVR0
	* MOV SVR3, SVR1
	*
	*/
	if (!sys) {
	}
	else if (sys == SYSTEM_CALL_READ) {
		/**
		* SYS 1,
		* STDIN,
		* __READ__
		*/
		uint32_t now_sv_register_num = get_now_register(*sce_bm_data);
		if (now_sv_register_num) {
			if (now_sv_register_num + system_call_read_args__ < SCE_VIRTUAL_REGISTER_RETURN) {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num > system_call_read_args__ ? system_call_read_args__ : now_sv_register_num, now_sv_register_num > system_call_read_args__ ? now_sv_register_num : system_call_read_args__, E_SCE_BINARY_INST_SAVE_REGISTER__));
			}
			else chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num, 0, E_SCE_BINARY_INST_SAVE_REGISTER_STACK__));
		}
		reset_sce_virtual_register(sce_bm_data);
		uint32_t svr0 = new_sce_virtual_register(sce_bm_data);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(svr0, sys, E_SCE_BINARY_INST_IMOV__));
		build_sce_binary_run_code_args(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sce_virtual_register(sce_bm_data), 0, E_SCE_BINARY_INST_SYSTEM_CALL__));
		if (now_sv_register_num) {
			if (now_sv_register_num + system_call_read_args__ < SCE_VIRTUAL_REGISTER_RETURN) {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num > system_call_read_args__ ? now_sv_register_num : system_call_read_args__, now_sv_register_num > system_call_read_args__ ? system_call_read_args__ : now_sv_register_num, E_SCE_BINARY_INST_LOAD_REGISTER__));
			}
			else {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num, 0, E_SCE_BINARY_INST_LOAD_REGISTER_STACK__));
			}
			set_sce_virtual_register(sce_bm_data, now_sv_register_num);

		}
		(void)new_sce_virtual_register(sce_bm_data);
		return SCE_VIRTUAL_REGISTER_RETURN;
	}
	else if (sys == SYSTEM_CALL_WRITE) {
		/*
		*
		*
		* __write__(1);
		* MOV SVR0 1
		* __write__(1, 1, 1);
		* MOV SVR1 1
		* MOV SVR2 2
		* MOV SVR3 3
		*
		* __write__(1+1+1, 1);
		* MOV SVR1 1
		* MOV SVR2 1
		* ADD SVR1 SVR2
		*
		*__write__(FILE + 1, BUF);
		* SVR0 SYSTEM_CALL_IDX
		* SVR1 FILE
		* SVR2 1
		*
		* obj i = 0;
		* i = i + 1;
		* ALLOC i
		* MOV SVR0, i
		* IMOV SVR1, 0
		* MOV SVR0, SVR1
		* RESET SVR
		* MOV SVR0, i
		* MOV SVR1, 1
		* ADD SVR0, SVR1
		*
		*
		*/
		uint32_t now_sv_register_num = get_now_register(*sce_bm_data);
		if (now_sv_register_num) {
			if (now_sv_register_num + system_call_write_args__ < SCE_VIRTUAL_REGISTER_RETURN) {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num > system_call_write_args__ ? system_call_write_args__ : now_sv_register_num, now_sv_register_num > system_call_write_args__ ? now_sv_register_num : system_call_write_args__, E_SCE_BINARY_INST_SAVE_REGISTER__));
			}
			else chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num, 0, E_SCE_BINARY_INST_SAVE_REGISTER_STACK__));
		}
		reset_sce_virtual_register(sce_bm_data);
		uint32_t svr0 = new_sce_virtual_register(sce_bm_data);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(svr0, sys, E_SCE_BINARY_INST_IMOV__));
		build_sce_binary_run_code_args(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sce_virtual_register(sce_bm_data), 0, E_SCE_BINARY_INST_SYSTEM_CALL__));
		if (now_sv_register_num) {
			if (now_sv_register_num + system_call_write_args__ < SCE_VIRTUAL_REGISTER_RETURN) {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num > system_call_write_args__ ? now_sv_register_num : system_call_write_args__, now_sv_register_num > system_call_write_args__ ? system_call_write_args__ : now_sv_register_num, E_SCE_BINARY_INST_LOAD_REGISTER__));
			}
			else {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num, 0, E_SCE_BINARY_INST_LOAD_REGISTER_STACK__));
			}
			set_sce_virtual_register(sce_bm_data, now_sv_register_num);

		}
		(void)new_sce_virtual_register(sce_bm_data);
		return SCE_VIRTUAL_REGISTER_RETURN;

		/*
		* rsp 256 - 10
		* R0 R1 R2 ...R255 R256 R257 R258
		* R256 = R0
		* R257 = R1
		* R258 = R2
		*
		* R0 = R256
		* R1 = R257
		* R2 = R258
		*
		*/
	}
	else if (sys == SYSTEM_CALL_OPEN) {

		uint32_t now_sv_register_num = get_now_register(*sce_bm_data);

		if (now_sv_register_num) {
			if (now_sv_register_num + system_call_open_args__ < SCE_VIRTUAL_REGISTER_RETURN)
				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num > system_call_write_args__ ? system_call_write_args__ : now_sv_register_num, now_sv_register_num > system_call_write_args__ ? now_sv_register_num : system_call_write_args__, E_SCE_BINARY_INST_SAVE_REGISTER__));

			else chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num, 0, E_SCE_BINARY_INST_SAVE_REGISTER_STACK__));
		}
		reset_sce_virtual_register(sce_bm_data);

		uint32_t svr0 = new_sce_virtual_register(sce_bm_data);

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(svr0, sys, E_SCE_BINARY_INST_IMOV__));
		build_sce_binary_run_code_args(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sce_virtual_register(sce_bm_data), 0, E_SCE_BINARY_INST_SYSTEM_CALL__));
		if (now_sv_register_num) {
			if (now_sv_register_num + system_call_open_args__ < SCE_VIRTUAL_REGISTER_RETURN) {
				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num > system_call_write_args__ ? now_sv_register_num : system_call_write_args__, now_sv_register_num > system_call_write_args__ ? system_call_write_args__ : now_sv_register_num, E_SCE_BINARY_INST_LOAD_REGISTER__));
			}
			else {

				chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sv_register_num, 0, E_SCE_BINARY_INST_LOAD_REGISTER_STACK__));
			}
			set_sce_virtual_register(sce_bm_data, now_sv_register_num);

		}

		return SCE_VIRTUAL_REGISTER_RETURN;
	}
	return 0;
}
static int build_opt_sce_binary_run_code_expr_ope(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {

	if (!ast) return -1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Assigment)) return 1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Add_Assigment)) return 1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Mul_Assigment)) return 1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Sub_Assigment)) return 1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Div_Assigment)) return 1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Mod_Assigment)) return 1;
	if (is_sce_ast_type(ast, E_Sce_Ast_Var_Decl)) return 1;

	if (is_sce_ast_type(ast, E_Sce_Ast_Call_Function)) {
		build_sce_binary_run_code_call_function(sce_bm_data, ast);
		return -2;
	}
	int i = build_opt_sce_binary_run_code_expr_ope(sce_bm_data, ast->left);
	int j = build_opt_sce_binary_run_code_expr_ope(sce_bm_data, ast->right);
	if (i == -2 || j == -2) {
		return -2;
	}
	if (i == 1 || j == 1) {
		return 1;
	}
	return -1;
}

Sce_Binary_Machine_Instructions* sce_start_build_binary_run_code(bool * is_error, Sce_Ast_Node* left) {
	if (!left) return NULL;
	Sce_Create_Binary_Machine_Code_Data sce_bm_data = { left, NULL, NULL, NULL, NULL, 0, false, 0, 0 };
	init_u8_string_buffers(&sce_bm_data.now_nest_name_, 20);
	build_sce_binary_run_code_module(&sce_bm_data);
	*is_error = sce_bm_data.is_error;

	if (sce_bm_data.sce_bmi_global_head) {
		chain_global_sce_bmi_of_create_bm(&sce_bm_data, gen_sce_bmi_s(0, exit_label, E_SCE_BINARY_INST_JMP__));
		chain_global_sce_bmi_of_create_bm(&sce_bm_data, gen_sce_bmi_s(0, exit_label, E_SCE_BINARY_INST_LABEL__));
		Sce_Binary_Machine_Instructions* res = sce_bm_data.sce_bmi_global;
		res->next = sce_bm_data.sce_bmi_head;
		return sce_bm_data.sce_bmi_global_head;
	}
	delete_u8_string_buffers(&sce_bm_data.now_nest_name_);
	return sce_bm_data.sce_bmi_head;
}
void build_sce_binary_run_code_module(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
	Sce_Ast_Node* right_ = sce_bm_data->ast_;
	chain_global_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, global_label, E_SCE_BINARY_INST_NAME__));
	chain_global_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(1, 0, E_SCE_BINARY_INST_NAME_REPEAT__));
	set_new_nest_object_chain(sce_bm_data, CHAIN_FILED_NAMES);

	while (right_) {
		if (is_sce_ast_module(right_)) build_sce_binary_run_code_toplevel_statement(sce_bm_data, right_->right);
		if (sce_bm_data->is_error) return;

		right_ = right_->left;
	}
	delete_now_nest(sce_bm_data);

}
void build_sce_binary_run_code_toplevel_statement(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return;
	if (is_sce_ast_type(ast, E_Sce_Ast_Name_Statement_Decl)) {
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, ast->data.buf, E_SCE_BINARY_INST_NAME__));
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(0, 0, E_SCE_BINARY_INST_NAME_REPEAT__));
		build_sce_binary_run_code_name(sce_bm_data, ast->right);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Expr)) {
		reset_sce_virtual_register(sce_bm_data);

		Sce_Binary_Machine_Instructions* old_inst_ = sce_bm_data->sce_bmi;
		if (!old_inst_) {
			(void)build_sce_binary_run_code_expr_ope(sce_bm_data, ast->left);
			old_inst_ = sce_bm_data->sce_bmi_head;

			if (!sce_bm_data->sce_bmi_global) {
				sce_bm_data->sce_bmi_global = sce_bm_data->sce_bmi;
				sce_bm_data->sce_bmi_global_head = old_inst_;
			}
			else {
				sce_bm_data->sce_bmi_global->next = old_inst_;
				sce_bm_data->sce_bmi_global = sce_bm_data->sce_bmi;
			}
			sce_bm_data->sce_bmi = sce_bm_data->sce_bmi_head = NULL;
			return;
		}
		else (void)build_sce_binary_run_code_expr_ope(sce_bm_data, ast->left);

		Sce_Binary_Machine_Instructions* tail_ = sce_bm_data->sce_bmi;
		Sce_Binary_Machine_Instructions* cache_ = old_inst_;
		old_inst_ = old_inst_->next;
		cache_->next = NULL;
		sce_bm_data->sce_bmi = cache_;
		if (!sce_bm_data->sce_bmi_global) {
			sce_bm_data->sce_bmi_global = tail_;
			sce_bm_data->sce_bmi_global_head = old_inst_;
		}
		else {
			sce_bm_data->sce_bmi_global->next = old_inst_;
			sce_bm_data->sce_bmi_global = tail_;
		}
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Error)) {
		printf(ast->data.buf);
		sce_bm_data->is_error = true;

	}
}
void build_sce_binary_run_code_name(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return;

	if (is_sce_ast_type(ast, E_Sce_Ast_Name_Statement)) {
		set_new_nest_object_chain(sce_bm_data, CHAIN_FILED_NAMES);

		Sce_Ast_Node* right_ = ast;
		Sce_Binary_Machine_Instructions* inst_ = NULL;
		Sce_Binary_Machine_Instructions* head_ = NULL;
		Sce_Binary_Machine_Instructions* tmp_ = sce_bm_data->sce_bmi;
		while (right_) {

			build_sce_binary_run_code_sections(&inst_, &head_, sce_bm_data, right_->right);
			if (sce_bm_data->is_error) break;

			right_ = right_->left;
		}
		Sce_Binary_Machine_Instructions* tail_ = sce_bm_data->sce_bmi;

		if (!head_) {
			head_ = gen_sce_bmi_s(0, exit_label, E_SCE_BINARY_INST_JMP__);
			inst_ = head_;

		}
		else {
			inst_->next = gen_sce_bmi_s(0, exit_label, E_SCE_BINARY_INST_JMP__);
			inst_ = inst_->next;
		}

		sce_bm_data->sce_bmi = tmp_;
		tmp_ = tmp_->next;
		sce_bm_data->sce_bmi->next = NULL;
		chain_sce_bmi_of_create_bm(sce_bm_data, head_);
		sce_bm_data->sce_bmi = inst_;
		if (tmp_) {
			chain_sce_bmi_of_create_bm(sce_bm_data, tmp_);
			sce_bm_data->sce_bmi = tail_;
		}
	ADD_HAS_PAHSE_IR:

		if (!SCE_HAS_PHASE_LABEL(sce_bm_data, HAS_START_LABEL)) {
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, start_label, E_SCE_BINARY_INST_LABEL__));
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, main_label, E_SCE_BINARY_INST_JMP__));
		}
		if (!SCE_HAS_PHASE_LABEL(sce_bm_data, HAS_MAIN_LABEL)) {
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, main_label, E_SCE_BINARY_INST_LABEL__));
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, main_label, E_SCE_BINARY_INST_JMP__));
		}
		if (!SCE_HAS_PHASE_LABEL(sce_bm_data, HAS_END_LABEL)) {
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, end_label, E_SCE_BINARY_INST_LABEL__));
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, exit_label, E_SCE_BINARY_INST_JMP__));

		}
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, exit_label, E_SCE_BINARY_INST_LABEL__));
		delete_now_nest(sce_bm_data);

	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Error)) {
		printf(ast->data.buf);
		sce_bm_data->is_error = true;

	}
}
void build_sce_binary_run_code_sections(Sce_Binary_Machine_Instructions ** init_inst_, Sce_Binary_Machine_Instructions** head_, Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return;
	sce_bm_data->sv_local_label = 0;
	empty_u8_byte_u8_string_bufferr(&sce_bm_data->now_nest_name_);
	sce_bm_data->now_function_name_size_ = 0;
	if (is_sce_ast_type(ast, E_Sce_Ast_Start_Section_Decl)) {
		Sce_Ast_Node* left_ = ast;
		SET_SCE_HAS_PHASE_LABEL(sce_bm_data, HAS_START_LABEL);
		/*
		* @:TODO
		* WHEN RELEASING, THE NUMBERING WILL BE CHANGED TO  A SEQUENTINAL FORMAT.
		*/
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, "%start", E_SCE_BINARY_INST_LABEL__));
		append_u8_byte_u8_string_basic(&sce_bm_data->now_nest_name_, start_label, START_LABEL_PREFIX_SIZE);
		set_new_nest_object_chain(sce_bm_data, CHAIN_FILED_NAMES);
		sce_bm_data->now_function_name_size_ = START_LABEL_PREFIX_SIZE;
		while (left_) {
			build_sce_binary_run_code_expr_statement(sce_bm_data, left_->right);
			if (sce_bm_data->is_error) return;

			left_ = left_->left;
		}
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, "%main", E_SCE_BINARY_INST_JMP__));
		delete_now_nest(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Main_Section_Decl)) {
		Sce_Ast_Node* left_ = ast;
		SET_SCE_HAS_PHASE_LABEL(sce_bm_data, HAS_MAIN_LABEL);

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, "%main", E_SCE_BINARY_INST_LABEL__));
		append_u8_byte_u8_string_basic(&sce_bm_data->now_nest_name_, main_label, MAIN_LABEL_PREFIX_SIZE);
		sce_bm_data->now_function_name_size_ = MAIN_LABEL_PREFIX_SIZE;
		set_new_nest_object_chain(sce_bm_data, CHAIN_FILED_NAMES);

		while (left_) {
			build_sce_binary_run_code_expr_statement(sce_bm_data, left_->right);
			if (sce_bm_data->is_error) return;

			left_ = left_->left;
		}
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, "%main", E_SCE_BINARY_INST_JMP__));
		delete_now_nest(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_End_Section_Decl)) {
		Sce_Ast_Node* left_ = ast;
		SET_SCE_HAS_PHASE_LABEL(sce_bm_data, HAS_END_LABEL);

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, "%end", E_SCE_BINARY_INST_LABEL__));
		append_u8_byte_u8_string_basic(&sce_bm_data->now_nest_name_, end_label, END_LABEL_PREFIX_SIZE);
		sce_bm_data->now_function_name_size_ = END_LABEL_PREFIX_SIZE;
		set_new_nest_object_chain(sce_bm_data, CHAIN_FILED_NAMES);

		while (left_) {
			build_sce_binary_run_code_expr_statement(sce_bm_data, left_->right);
			if (sce_bm_data->is_error) return;

			left_ = left_->left;
		}
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, "%exit", E_SCE_BINARY_INST_JMP__));
		delete_now_nest(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Expr)) {

		reset_sce_virtual_register(sce_bm_data);

		Sce_Binary_Machine_Instructions* old_inst_ = sce_bm_data->sce_bmi;

		build_sce_binary_run_code_expr_ope(sce_bm_data, ast->left);
		if (!old_inst_) {
			old_inst_ = sce_bm_data->sce_bmi_head;
			if (!sce_bm_data->sce_bmi_head) return;
		}
		Sce_Binary_Machine_Instructions* tail_ = sce_bm_data->sce_bmi;

		Sce_Binary_Machine_Instructions* cache_ = old_inst_;

		old_inst_ = old_inst_->next;
		cache_->next = NULL;
		sce_bm_data->sce_bmi = cache_;
		if (*init_inst_) {
			(*init_inst_)->next = old_inst_;
			(*init_inst_) = tail_;
		}
		else {
			(*init_inst_) = tail_;
			(*head_) = old_inst_;
		}
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Error)) {
		printf(ast->data.buf);
		sce_bm_data->is_error = true;

	}
	else {

	}
}

void build_sce_binary_run_code_decl(Sce_Create_Binary_Machine_Code_Data* sce_bm_data) {
}
/*
* A = 1 + 1;
* MOV RAX 1;
* ADD RAX, 1
* MOV A, RAX
*/
static void set_sce_append_nest_name(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, const uint8_t * nest_name) {
	int i = snprintf(sce_bm_data->sv_local_label_buf, CONST_128_BYTE, nest_name, sce_bm_data->sv_local_label);
	append_u8_byte_u8_string_basic(&sce_bm_data->now_nest_name_, sce_bm_data->sv_local_label_buf, (size_t)i);
}
void build_sce_binary_run_code_condition_expr(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	uint32_t ope1 = build_sce_binary_run_code_basic_iden(sce_bm_data, ast);
	set_sce_append_nest_name(sce_bm_data, if_label_prefix);
	if (ope1 == SCE_VIRTUAL_REGISTER_CMP_RETURN) {
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, sce_bm_data->now_nest_name_.str__, E_SCE_BINARY_INST_JMP__));
	}
	else {
		uint32_t ope2 = new_sce_virtual_register(sce_bm_data);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(ope2, 0, E_SCE_BINARY_INST_IMOV__));

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(ope1, ope2, E_SCE_BINARY_INST_EQ__));
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, sce_bm_data->now_nest_name_.str__, E_SCE_BINARY_INST_JMP__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}
}
uint32_t build_sce_binary_run_code_expr_statement(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	reset_sce_virtual_register(sce_bm_data);
	if (!ast) return 0;
	if (is_sce_ast_type(ast, E_Sce_Ast_Expr)) {

		int res = build_opt_sce_binary_run_code_expr_ope(sce_bm_data, ast->left);
		//printf("%d", res);
		if (res == -2) {
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(SCE_VIRTUAL_REGISTER_RETURN, 0, E_SCE_BINARY_INST_DELETE_REGISTER__));
			return 0;
		}
		else if(res == -1)return 0;
		else build_sce_binary_run_code_expr_ope(sce_bm_data, ast->left);
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_If)) {
		build_sce_binary_run_code_condition_expr(sce_bm_data, ast->left);
		Sce_Binary_Machine_Instructions* inst_ = gen_sce_bmi_s(0, sce_bm_data->now_nest_name_.str__, E_SCE_BINARY_INST_LABEL__);
		set_new_nest_object_chain(sce_bm_data, &inst_->scope2[sce_bm_data->now_function_name_size_]);
		empty_from_pos_u8_byte_u8_string_bufferr(&sce_bm_data->now_nest_name_, (uint32_t)sce_bm_data->now_function_name_size_);

		sce_bm_data->sv_local_label++;
		Sce_Ast_Node* block_ = ast->right;
		while (block_) {
			(void)build_sce_binary_run_code_expr_statement(sce_bm_data, block_->right);
			if (sce_bm_data->is_error) break;
			block_ = block_->left;
		}
		chain_sce_bmi_of_create_bm(sce_bm_data, inst_);

		delete_now_nest(sce_bm_data);
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Error)) {
		printf(ast->data.buf);
		sce_bm_data->is_error = true;

	}
	return 0;
}

void build_sce_binary_run_code_args(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return;
	/*
	* ARGS----ARGS----IDEN
	*  |        |-----IDEN
	*  |-----IDEN
	*/
	/*
	*
	* ARGS
	*  |
	*  |----MUL---IDEN1
	*  |    |-----IDEN2
	*  |
	*  |-----ADD---1
	*         |-----2
	*/
	if (is_sce_ast_type(ast, E_Sce_Ast_Args)) {
		
		(void)build_sce_binary_run_code_args(sce_bm_data, ast->left);
		build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		return;
	}
	build_sce_binary_run_code_basic_iden(sce_bm_data, ast);

}
uint32_t build_sce_binary_run_code_expr_ope(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast)return 0;

	if (is_sce_ast_type(ast, E_Sce_Ast_Call_Function)) {
		return build_sce_binary_run_code_call_function(sce_bm_data, ast);
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Equal)) {
		/*
		* 3 OPERAND
		* CMP SVR0, SVR1, SVR2
		* 2 OPERAND
		* EQ SVR0, SVR1
		* RESULT -> RET 129
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_EQ__));
		delete_sce_virtual_registers(sce_bm_data, 1);
		return SCE_VIRTUAL_REGISTER_CMP_RETURN;
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Not_Equal)) {
		/*
		* 3 OPERAND
		* CMP SVR0, SVR1, SVR2
		* 2 OPERAND
		* EQ SVR0, SVR1
		* RESULT -> RET 129
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_NEQ__));
		//chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(right_, SCE_VIRTUAL_REGISTER_CMP_RETURN, E_SCE_BINARY_INST_RMOV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
		return SCE_VIRTUAL_REGISTER_CMP_RETURN;

	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Less_Equal)) {
		/*
		* 3 OPERAND
		* CMP SVR0, SVR1, SVR2
		* 2 OPERAND
		* EQ SVR0, SVR1
		* RESULT -> RET 129
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_LSEQ__));
		//chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(right_, SCE_VIRTUAL_REGISTER_CMP_RETURN, E_SCE_BINARY_INST_RMOV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
		return SCE_VIRTUAL_REGISTER_CMP_RETURN;

	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Grater_Equal)) {
		/*
		* 3 OPERAND
		* CMP SVR0, SVR1, SVR2
		* 2 OPERAND
		* EQ SVR0, SVR1
		* RESULT -> RET 129
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_GTEQ__));
		//chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(right_, SCE_VIRTUAL_REGISTER_CMP_RETURN, E_SCE_BINARY_INST_RMOV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
		return SCE_VIRTUAL_REGISTER_CMP_RETURN;

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Less)) {
		/*
		* 3 OPERAND
		* CMP SVR0, SVR1, SVR2
		* 2 OPERAND
		* EQ SVR0, SVR1
		* RESULT -> RET 129
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_LS__));
		//chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(right_, SCE_VIRTUAL_REGISTER_CMP_RETURN, E_SCE_BINARY_INST_RMOV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
		return SCE_VIRTUAL_REGISTER_CMP_RETURN;

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Grater)) {
		/*
		* 3 OPERAND
		* CMP SVR0, SVR1, SVR2
		* 2 OPERAND
		* EQ SVR0, SVR1
		* RESULT -> RET 129
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_GT__));
		///chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(right_, SCE_VIRTUAL_REGISTER_CMP_RETURN, E_SCE_BINARY_INST_RMOV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
		return SCE_VIRTUAL_REGISTER_CMP_RETURN;

	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Var_Decl)) {
		uint8_t* value_ = (uint8_t*)ast->data.buf;

		sce_bm_data->is_error = set_new_object_name(sce_bm_data, value_);
		if (sce_bm_data->is_error) {
			printf(already_name_value_error, value_);
			sce_bm_data->is_error = true;

			return 0;
		}
		uint8_t* nest_value_ = new_nest_symbol_name(value_, sce_bm_data->now_->name_);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, nest_value_, E_SCE_BINARY_INST_ALLOC__));

		sce_bm_data->is_error = false;
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Assigment)) {
		/*
		* i = 1 + 1;
		* MOV SVR0, &i
		* MOV SVR1, 1
		* MOV SVR2, 1
		* ADD SVR1, SVR2
		* MOV SVR0, SVR1
		*/

		uint32_t left_ = build_sce_binary_run_code_basic_symbol_iden_and_decl(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		if (sce_bm_data->is_error) return 0;
		
		/*
		* i = 0;
		* MOV SVR0, &i
		* MOV SVR1, 0
		* MOV SVR0, SVR1
		*/
		//uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_RMOV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Add_Assigment)) {
		/*
		* I+=0;
		* I = I + 0;
		* MOV SVR0 &I
		* MOV SVR1 0
		* ADD SVR0 SVR1
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_symbol_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);

		if (sce_bm_data->is_error) return 0;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_ADD__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Sub_Assigment)) {
		/*
		* I+=0;
		* I = I + 0;
		* MOV SVR0 &I
		* MOV SVR1 0
		* ADD SVR0 SVR1
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_symbol_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		if (sce_bm_data->is_error) return 0;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_SUB__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Mul_Assigment)) {
		/*
		* I+=0;
		* I = I + 0;
		* MOV SVR0 &I
		* MOV SVR1 0
		* ADD SVR0 SVR1
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_symbol_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		if (sce_bm_data->is_error) return 0;

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_MUL__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Div_Assigment)) {
		/*
		* I+=0;
		* I = I + 0;
		* MOV SVR0 &I
		* MOV SVR1 0
		* ADD SVR0 SVR1
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_symbol_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		if (sce_bm_data->is_error) return 0;

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_DIV__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Mod_Assigment)) {
		/*
		* I+=0;
		* I = I + 0;
		* MOV SVR0 &I
		* MOV SVR1 0
		* ADD SVR0 SVR1
		*/
		uint32_t left_ = build_sce_binary_run_code_basic_symbol_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		if (sce_bm_data->is_error) return 0;

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_MOD__));
		delete_sce_virtual_registers(sce_bm_data, 1);
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Left_Shift)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_SHL__));
		delete_sce_virtual_register(sce_bm_data);


	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Right_Shift)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_SHR__));
		delete_sce_virtual_register(sce_bm_data);


		}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Bit_And)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_AND__));
		delete_sce_virtual_register(sce_bm_data);


	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Bit_Or)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_OR__));
		delete_sce_virtual_register(sce_bm_data);

	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Bit_Xor)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_XOR__));
		delete_sce_virtual_register(sce_bm_data);

	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Bit_Not)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_NOT__));
		delete_sce_virtual_register(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Add)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_ADD__));
		delete_sce_virtual_register(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Sub)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);
		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_SUB__));
		delete_sce_virtual_register(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Mul)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_MUL__));
		delete_sce_virtual_register(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Div)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_DIV__));
		delete_sce_virtual_register(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Mod)) {
		uint32_t left_ = build_sce_binary_run_code_basic_const_iden(sce_bm_data, ast->left);

		uint32_t right_ = build_sce_binary_run_code_basic_iden(sce_bm_data, ast->right);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(left_, right_, E_SCE_BINARY_INST_MOD__));
		delete_sce_virtual_register(sce_bm_data);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Error)) {
		printf(ast->data.buf);
		sce_bm_data->is_error = true;

	}
	return sce_bm_data->sv_registers;
}
/*
* ADD-----ADD-----------ADD---iden
*		|----iden	       |---iden
*
* MOV SVR0 0
* MOV SVR0 1
* MOV SVR1 A
* 4byte 4byte 1byte
**/

uint32_t build_sce_binary_run_code_basic_symbol_iden(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return 0;
	if (is_sce_ast_type(ast, E_Sce_Ast_Iden)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t* value_ = (uint8_t*)ast->data.buf;
		Nest_Object_Chain* chain_ = search_object_name_in_nest_object_chain(sce_bm_data, value_);
		if (!chain_) {
			printf(not_defined_name_value_error, value_);
			sce_bm_data->is_error = true;
			return 0;
		}
		uint8_t* nest_value_ = new_nest_symbol_name(value_, chain_->name_);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(ope1, nest_value_, E_SCE_BINARY_INST_MOV__));
		sce_bm_data->is_error = false;
		return ope1;
	}
	printf(left_hand_value_error);

	sce_bm_data->is_error = true;

	return 0;
}

uint32_t build_sce_binary_run_code_basic_symbol_iden_and_decl(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return 0;
	if (is_sce_ast_type(ast, E_Sce_Ast_Iden)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t* value_ = (uint8_t*)ast->data.buf;

		Nest_Object_Chain * chain_ = search_object_name_in_nest_object_chain(sce_bm_data, value_);

		if (!chain_) {
			printf(not_defined_name_value_error, value_);
			sce_bm_data->is_error = true;
			return 0;
		}
		uint8_t* nest_value_ = new_nest_symbol_name(value_, chain_->name_);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_mov_s(ope1, nest_value_, E_SCE_BINARY_INST_MOV__));
		sce_bm_data->is_error = false;
		return ope1;
	}
	/**
	* %STARTIF0{
	*	%STARTIF1{
	*	
	*	}
	* }
	* 
	*/
	if (is_sce_ast_type(ast, E_Sce_Ast_Var_Decl)) {
		uint8_t* value_ = (uint8_t*)ast->data.buf;

		sce_bm_data->is_error = set_new_object_name(sce_bm_data, value_);
		if (sce_bm_data->is_error) {
			printf(already_name_value_error, value_);
			sce_bm_data->is_error = true;

			return 0;
		}
		uint8_t* nest_value_ = new_nest_symbol_name(value_, sce_bm_data->now_->name_);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(0, nest_value_, E_SCE_BINARY_INST_ALLOC__));

		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_mov_s(ope1, nest_value_, E_SCE_BINARY_INST_MOV__));
		sce_bm_data->is_error = false;

		return ope1;
	}
	printf(left_hand_value_error);

	sce_bm_data->is_error = true;

	return 0;
}

uint32_t build_sce_binary_run_code_basic_iden(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return 0;
	if (is_sce_ast_type(ast, E_Sce_Ast_Bool_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		bool value_ = *(bool*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_b(ope1, value_, E_SCE_BINARY_INST_BMOV__));
		return sce_bm_data->sv_registers;
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Char_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t value_ = *(uint8_t*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_c(ope1, value_, E_SCE_BINARY_INST_CMOV__));
		return sce_bm_data->sv_registers;
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Int_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint32_t value_ = *(uint32_t*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(ope1, value_, E_SCE_BINARY_INST_IMOV__));
		return sce_bm_data->sv_registers;
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Float_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);

		int64_t value_ = *(int64_t*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(ope1, value_, E_SCE_BINARY_INST_FMOV__));
		return sce_bm_data->sv_registers;
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_String_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t* value_ = (uint8_t*)ast->data.buf;
		ast->data.buf = NULL;

		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_mov_s(ope1, value_, E_SCE_BINARY_INST_SMOV__));
		return sce_bm_data->sv_registers;
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Iden)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t* value_ = (uint8_t*)ast->data.buf;

		/*
		* MOV SVR, &I
		* VAR IS ALWAYS A REFERENCE COPY
		*/

		Nest_Object_Chain* chain_ = search_object_name_in_nest_object_chain(sce_bm_data, value_);
		if (!chain_) {
			printf(not_defined_name_value_error, value_);
			sce_bm_data->is_error = true;
			return 0;
		}
		uint8_t* nest_value_ = new_nest_symbol_name(value_, chain_->name_);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(ope1, nest_value_, E_SCE_BINARY_INST_MOV__));
		return sce_bm_data->sv_registers;
	}
	else return build_sce_binary_run_code_expr_ope(sce_bm_data, ast);
}

uint32_t build_sce_binary_run_code_basic_const_iden(Sce_Create_Binary_Machine_Code_Data* sce_bm_data, Sce_Ast_Node* ast) {
	if (!ast) return 0;

	if (is_sce_ast_type(ast, E_Sce_Ast_Bool_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		bool value_ = *(bool*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_b(ope1, value_, E_SCE_BINARY_INST_BMOV__));
		return sce_bm_data->sv_registers;
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Char_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t value_ = *(uint8_t*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_c(ope1, value_, E_SCE_BINARY_INST_CMOV__));
		return sce_bm_data->sv_registers;
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Int_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		int64_t value_ = *(int64_t*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_i(ope1, value_, E_SCE_BINARY_INST_IMOV__));
		return sce_bm_data->sv_registers;
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Float_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);

		float value_ = *(float*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_f(ope1, value_, E_SCE_BINARY_INST_FMOV__));
		return sce_bm_data->sv_registers;
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_Double_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);

		double value_ = *(double*)ast->data.buf;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_d(ope1, value_, E_SCE_BINARY_INST_FMOV__));
		return sce_bm_data->sv_registers;
	}

	else if (is_sce_ast_type(ast, E_Sce_Ast_String_Value)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t* value_ = (uint8_t*)ast->data.buf;
		ast->data.buf = NULL;
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_mov_s(ope1, value_, E_SCE_BINARY_INST_SMOV__));
		return sce_bm_data->sv_registers;
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Iden)) {
		uint32_t ope1 = new_sce_virtual_register(sce_bm_data);
		uint8_t* value_ = (uint8_t*)ast->data.buf;

		/*
		* MOV SVR, VAR
		* IF THE VARIABLE IS A PRIMITIVE TYPE, THEN PERFORM A DEEP COPY.
		* ELSE, PERFORM A REFERENCE COPY.
		*/

		Nest_Object_Chain* chain_ = search_object_name_in_nest_object_chain(sce_bm_data, value_);
		if (!chain_) {
			printf(not_defined_name_value_error, value_);
			sce_bm_data->is_error = true;
			return 0;
		}
		uint8_t* nest_value_ = new_nest_symbol_name(value_, chain_->name_);
		chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi_s(ope1, nest_value_, E_SCE_BINARY_INST_MMOV__));
		return sce_bm_data->sv_registers;
	}
	else {
		uint32_t left_ = build_sce_binary_run_code_expr_ope(sce_bm_data, ast);
		if (is_register_return(left_)) {
			chain_sce_bmi_of_create_bm(sce_bm_data, gen_sce_bmi(now_sce_virtual_register(sce_bm_data), left_, E_SCE_BINARY_INST_RMOV__));
			left_ = now_sce_virtual_register(sce_bm_data);
		}
		return left_;
	}
}
