#ifndef _SCE_PARSER_H_
#define _SCE_PARSER_H_
#include "sce_lexer.h"
#define SET_SCE_AST_TYPE(CONVERT_FUNC)\
CONVERT_FUNC(E_Sce_Ast_NULL),\
CONVERT_FUNC(E_Sce_Ast_Return),\
CONVERT_FUNC(E_Sce_Ast_Name_Meta),\
CONVERT_FUNC(E_Sce_Ast_Name_Repeat),\
CONVERT_FUNC(E_Sce_Ast_Name_Once),\
CONVERT_FUNC(E_Sce_Ast_Instance_Obj),\
CONVERT_FUNC(E_Sce_Ast_Instance_Parameter),\
CONVERT_FUNC(E_Sce_Ast_Call_Function),\
CONVERT_FUNC(E_Sce_Ast_Function_Decl),\
CONVERT_FUNC(E_Sce_Ast_Parameter_Args_Decl),\
CONVERT_FUNC(E_Sce_Ast_Args),\
CONVERT_FUNC(E_Sce_Ast_Global_Modules_Statement),\
CONVERT_FUNC(E_Sce_Ast_Modules_Statement),\
CONVERT_FUNC(E_Sce_Ast_Expr),\
CONVERT_FUNC(E_Sce_Ast_Pare),\
CONVERT_FUNC(E_Sce_Ast_Assigment),\
CONVERT_FUNC(E_Sce_Ast_Add_Assigment),\
CONVERT_FUNC(E_Sce_Ast_Sub_Assigment),\
CONVERT_FUNC(E_Sce_Ast_Mul_Assigment),\
CONVERT_FUNC(E_Sce_Ast_Div_Assigment),\
CONVERT_FUNC(E_Sce_Ast_Mod_Assigment),\
CONVERT_FUNC(E_Sce_Ast_Add),\
CONVERT_FUNC(E_Sce_Ast_Sub),\
CONVERT_FUNC(E_Sce_Ast_Mul),\
CONVERT_FUNC(E_Sce_Ast_Div),\
CONVERT_FUNC(E_Sce_Ast_Mod),\
CONVERT_FUNC(E_Sce_Ast_Equal),\
CONVERT_FUNC(E_Sce_Ast_Not_Equal),\
CONVERT_FUNC(E_Sce_Ast_Left_Shift),\
CONVERT_FUNC(E_Sce_Ast_Right_Shift),\
CONVERT_FUNC(E_Sce_Ast_Less),\
CONVERT_FUNC(E_Sce_Ast_Grater),\
CONVERT_FUNC(E_Sce_Ast_Less_Equal),\
CONVERT_FUNC(E_Sce_Ast_Grater_Equal),\
CONVERT_FUNC(E_Sce_Ast_Bit_And),\
CONVERT_FUNC(E_Sce_Ast_Bit_Or),\
CONVERT_FUNC(E_Sce_Ast_Bit_Xor),\
CONVERT_FUNC(E_Sce_Ast_Bit_Not),\
CONVERT_FUNC(E_Sce_Ast_Bit_Not_Logic),\
CONVERT_FUNC(E_Sce_Ast_Member_Acess),\
CONVERT_FUNC(E_Sce_Ast_Expr_Comma),\
CONVERT_FUNC(E_Sce_Ast_Start_Section_Decl),\
CONVERT_FUNC(E_Sce_Ast_Main_Section_Decl),\
CONVERT_FUNC(E_Sce_Ast_End_Section_Decl),\
CONVERT_FUNC(E_Sce_Ast_Start_Call_Section),\
CONVERT_FUNC(E_Sce_Ast_Main_Call_Section),\
CONVERT_FUNC(E_Sce_Ast_End_Call_Section),\
CONVERT_FUNC(E_Sce_Ast_Exit_Call_Section),\
CONVERT_FUNC(E_Sce_Ast_Name_Statement),\
CONVERT_FUNC(E_Sce_Ast_Name_Statement_Decl),\
CONVERT_FUNC(E_Sce_Ast_Bool_Value),\
CONVERT_FUNC(E_Sce_Ast_Char_Value),\
CONVERT_FUNC(E_Sce_Ast_Int_Value),\
CONVERT_FUNC(E_Sce_Ast_Float_Value),\
CONVERT_FUNC(E_Sce_Ast_Double_Value),\
CONVERT_FUNC(E_Sce_Ast_String_Value),\
CONVERT_FUNC(E_Sce_Ast_Iden),\
CONVERT_FUNC(E_Sce_Ast_Var_Decl),\
CONVERT_FUNC(E_Sce_Ast_If),\
CONVERT_FUNC(E_Sce_Ast_If_Section),\
CONVERT_FUNC(E_Sce_Ast_Else_If),\
CONVERT_FUNC(E_Sce_Ast_Else_If_Section),\
CONVERT_FUNC(E_Sce_Ast_Else),\
CONVERT_FUNC(E_Sce_Ast_Else_Section),\
CONVERT_FUNC(E_Sce_Ast_Error)

#define E_SCE_AST_STSRING_OFFSET 10
#define GET_SCE_AST_STRING(ast) debug_sce_ast_strs[ast->data.ast_type] + E_SCE_AST_STSRING_OFFSET
#define CONVERT_DEFINE_STR(name) #name
#define CONVERT_DEFINE(name) name
typedef enum Sce_Ast_Type {
	SET_SCE_AST_TYPE(CONVERT_DEFINE)
}Sce_Ast_Type;

static const uint8_t* debug_sce_ast_strs[] = {
	SET_SCE_AST_TYPE(CONVERT_DEFINE_STR)
};
union Sce_Buffers {
	bool bv;/*bool*/
	uint8_t cv;/*char*/
	uint32_t vi;/*int*/
	float vf;/*float*/
	double vd;/*double*/
	void* vv;/*virtual*/
	uint8_t* sv;/*string*/

};
typedef struct Sce_Ast_Node {
	struct Sce_Ast_Node* left;
	struct Data { void* buf;  Sce_Ast_Type ast_type; }data;
	struct Sce_Ast_Node* right;
}Sce_Ast_Node;


sce_si_strap Sce_Ast_Type get_sce_ast_type(Sce_Ast_Node* left) {
	return left->data.ast_type;
}
sce_si_strap bool is_sce_ast_type(Sce_Ast_Node* left, Sce_Ast_Type sce_ast_type) {
	return get_sce_ast_type(left) == sce_ast_type;
}

sce_si_strap bool is_sce_ast_module(Sce_Ast_Node* left) {
	return is_sce_ast_type(left, E_Sce_Ast_Global_Modules_Statement);
}
sce_si_strap Sce_Ast_Node* create_sce_ast_node(Sce_Ast_Node * left_, Sce_Ast_Node * right_, const void * buf_, uint32_t size_, Sce_Ast_Type type_) {
	Sce_Ast_Node* ast_ = smart_malloc(Sce_Ast_Node, 1);
	if (!ast_) return NULL;
	ast_->left = left_;
	ast_->right = right_;
	ast_->data.buf = dupalloc(buf_, size_);
	ast_->data.ast_type = type_;
	return ast_;
}

sce_si_strap void set_up_sce_ast_node(Sce_Ast_Node* left_, const void* buf_, uint32_t size_, Sce_Ast_Type type_) {
	left_->data.buf = dupalloc(buf_, size_);
	left_->data.ast_type = type_;
}

sce_si_strap void delete_sce_ast_node(Sce_Ast_Node* left) {
	if (!left) return;

	if (
		is_sce_ast_type(left, E_Sce_Ast_Start_Section_Decl) ||
		is_sce_ast_type(left, E_Sce_Ast_Main_Section_Decl) ||
		is_sce_ast_type(left, E_Sce_Ast_End_Section_Decl) ||
		is_sce_ast_type(left, E_Sce_Ast_Name_Statement)
		) {

		Sce_Ast_Node* l = left;
		while (l) {
			Sce_Ast_Node* next = l->left;

			delete_sce_ast_node(l->right);

			if (l->data.buf) {
				free(l->data.buf);
				l->data.buf = NULL;
			}

			s_free(l);
			l = next;
		}
		return;
	}

	delete_sce_ast_node(left->left);
	delete_sce_ast_node(left->right);

	if (left->data.buf) {
		s_free(left->data.buf);
		left->data.buf = NULL;
	}

	s_free(left);
}
Sce_Ast_Node* create_sce_value_literal_ast(Sce_Lexer_Token* sce_lexer_token);

Sce_Ast_Type get_lexer_expect_consume_sce(
	Sce_Lexer_Type lexer_type,
	Sce_Lexer_Token_Lists* sce_lexer_token_lists,
	const uint8_t** sce_ast_tokes,
	const Sce_Ast_Type* sce_ast_types,
	const uint32_t size_
);

Sce_Ast_Type get_expect_consume_sce_mul_div_mod(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Type get_expect_consume_sce_add_sub(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Type get_expect_consume_sce_assigment(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Type get_expect_consume_sce_shifts(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Type get_expect_consume_sce_equals(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Type get_expect_consume_sce_relatives(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_return(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* start_build_sce_ast(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_module(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

int get_end_block_process(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_expr_statement(Sce_Ast_Node* ast, Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_toplevel_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_ast_sce_block(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_null_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_base_keyword(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_keyword(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_toplevel_keyword(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_name_statement(
	int* error_code,
	Sce_Lexer_Token_Lists* sce_lexer_token_lists
);

Sce_Ast_Node* build_sce_ast_name_field_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_call_section_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_call_section(
	Sce_Ast_Type sce_ast_call_section_type,
	Sce_Lexer_Token_Lists* sce_lexer_token_lists
);

Sce_Ast_Node* build_sce_ast_section_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_block(
	Sce_Ast_Type sce_ast_section_type,
	Sce_Lexer_Token_Lists* sce_lexer_token_lists
);
Sce_Ast_Node* build_sce_ast_base_block(Sce_Ast_Type sce_ast_section_type, Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_block_process(
	int* error_code,
	Sce_Lexer_Token_Lists* sce_lexer_token_lists,
	Sce_Ast_Type sce_ast_block_type
);
Sce_Ast_Node* build_sce_ast_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_if_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type if_else_type);
Sce_Ast_Node* build_sce_ast_base_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_base_if_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type if_else_type);
Sce_Ast_Node* build_sce_ast_base_if(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_if(Sce_Lexer_Token_Lists* sce_lexer_token_lists);
Sce_Ast_Node* build_sce_ast_base_block_process(int* error_code, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type sce_ast_block_type);

Sce_Ast_Node* build_sce_ast_iden_decl(Sce_Lexer_Token_Lists* sce_lexer_token_lists);

Sce_Ast_Node* build_sce_ast_expr_conmma(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_assigment(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_bit_or(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_bit_xor(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_bit_and(Sce_Lexer_Token_Lists* src_ans_token_list);

Sce_Ast_Node* build_sce_ast_equals(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_relatives(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_shifts(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_add_sub(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_mul_div_mod(Sce_Lexer_Token_Lists* src_ans_token_list);

Sce_Ast_Node* build_sce_ast_sign_symbol(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_memebers(Sce_Lexer_Token_Lists* src_ans_token_list);

Sce_Ast_Node* build_sce_ast_args_functions(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_call_functions(Sce_Lexer_Token_Lists* src_ans_token_list);

Sce_Ast_Node* build_sce_ast_pare(Sce_Lexer_Token_Lists* src_ans_token_list);
Sce_Ast_Node* build_sce_ast_basic_token(Sce_Lexer_Token_Lists* src_ans_token_list);
#endif // !_SCE_PARSER_H_
