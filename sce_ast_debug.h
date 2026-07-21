#ifndef SCE_AST_DEBUG_H_
#define SCE_AST_DEBUG_H_
#include "sce_parser.h"
#define SCE_AST_DEBUG_V_BRANCH          "„    "
#define SCE_AST_DEBUG_EMPTY_BRANCH       "    "
#define SCE_AST_DEBUG_SPLIT_END_BRANCH   "„¤„Ÿ„Ÿ "
#define SCE_AST_DEBUG_SPLIT_NEXT_BRANCH  "„¥„Ÿ„Ÿ "

sce_si_strap void print_ast_debug(Sce_Ast_Node* ast, const uint8_t * prefix, bool last_check) {
	if (!ast)return;

	fputs((const char *)prefix, stdout);
	fputs(last_check ? SCE_AST_DEBUG_SPLIT_END_BRANCH : SCE_AST_DEBUG_SPLIT_NEXT_BRANCH, stdout);
	if (
		
		is_sce_ast_type(ast, E_Sce_Ast_Iden) ||
		is_sce_ast_type(ast, E_Sce_Ast_Var_Decl) ||
		is_sce_ast_type(ast, E_Sce_Ast_Function_Decl) ||
		is_sce_ast_type(ast, E_Sce_Ast_Name_Statement_Decl)
		) {

		printf("(\x1b[31m%s\x1b[0m)", (const uint8_t*)ast->data.buf);
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Bool_Value)) {
		printf("(\x1b[32m%s\x1b[0m)", 
			*(const bool*)ast->data.buf ?
			"True" : 
			"False"
		);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Char_Value)) {
		printf("(\x1b[33m%c\x1b[0m)", *(const char*)ast->data.buf);
	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_Int_Value)) {
		printf("(\x1b[33m%lld\x1b[0m)", *(const int64_t*)ast->data.buf);

	}
	else if (is_sce_ast_type(ast, E_Sce_Ast_String_Value)) {
		printf("(\x1b[33m%s\x1b[0m)", (const uint8_t*)ast->data.buf);
	}

	else printf("%s", GET_SCE_AST_STRING(ast));
	putchar('\n');

	uint8_t next_prefix[256];

	snprintf((char* const)next_prefix, sizeof(next_prefix) / sizeof(next_prefix[0]), "%s%s", prefix, last_check ? SCE_AST_DEBUG_EMPTY_BRANCH : SCE_AST_DEBUG_V_BRANCH);

	if (
		is_sce_ast_type(ast, E_Sce_Ast_Start_Section_Decl) ||
		is_sce_ast_type(ast, E_Sce_Ast_Main_Section_Decl) ||
		is_sce_ast_type(ast, E_Sce_Ast_End_Section_Decl) ||
		is_sce_ast_type(ast, E_Sce_Ast_Name_Statement) ||
		is_sce_ast_type(ast, E_Sce_Ast_If_Section) ||
		is_sce_ast_type(ast, E_Sce_Ast_Else_If_Section) ||
		is_sce_ast_type(ast, E_Sce_Ast_Else_Section)

		) {
		uint32_t i = 1;
		for (Sce_Ast_Node* l = ast; l; l = l->left) i++;
		uint32_t j = 1;
		for (Sce_Ast_Node* l = ast; l; l = l->left, j++) print_ast_debug(l->right, next_prefix, (j == i - 1));
		return;
	}
	if (is_sce_ast_type(ast, E_Sce_Ast_Args)) {
		for (Sce_Ast_Node* l = ast; l; l = l->left) {
			if (l->data.ast_type != E_Sce_Ast_Args) { print_ast_debug(l, next_prefix, true); break; }
			else print_ast_debug(l->right, next_prefix, false);

		}
		return;
	}
	if (ast->left && ast->right) {
		print_ast_debug(ast->left, next_prefix, false);
		print_ast_debug(ast->right, next_prefix, true);
	}
	else if (ast->left) {
		print_ast_debug(ast->left, next_prefix, true);
	}
	else print_ast_debug(ast->right, next_prefix, true);

}

sce_si_strap void start_print_ast_debug(Sce_Ast_Node* ast) {
	if (!ast) return;
	Sce_Ast_Node* left_ = ast;
	if (is_sce_ast_module(left_)) {
		printf("%s", GET_SCE_AST_STRING(ast));
		putchar('\n');
		uint32_t i = 1;
		for (Sce_Ast_Node* l = ast; l; l = l->left) i++;
		uint32_t j = 1;
		/*
		* 9
		* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
		*/

		for (Sce_Ast_Node* l = ast; l; l = l->left, j++) {
			print_ast_debug(l->right, "", (j == i - 1));
		}
	}

}

sce_si_strap void normal_debug_sce_ast_print(Sce_Ast_Node* ast, uint32_t src_idx) {
	if (!ast)return;
	normal_debug_sce_ast_print(ast->left, src_idx + 1);
	for (uint32_t i = 0; i < src_idx; ++i)putchar('.');
	printf("%s", GET_SCE_AST_STRING(ast));
	if (ast->data.buf) {
		if (ast->data.ast_type == E_Sce_Ast_Iden) printf("(%s)", (uint8_t*)ast->data.buf);
		if (ast->data.ast_type == E_Sce_Ast_Var_Decl) printf("(%s)", (uint8_t*)ast->data.buf);
		else if (ast->data.ast_type == E_Sce_Ast_Int_Value) printf("(%llu)", *(uint64_t*)ast->data.buf);

	}
	putchar('\n');
	normal_debug_sce_ast_print(ast->right, src_idx + 1);
}
#endif // SCE_AST_DEBUG_H_
