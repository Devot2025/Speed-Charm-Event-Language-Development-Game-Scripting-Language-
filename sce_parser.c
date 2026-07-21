#include "sce_parser.h"

Sce_Ast_Node* create_sce_bool_literal_ast(Sce_Lexer_Token* sce_lexer_token) {
	bool bool_value = true;
	if (simple_strcmp(sce_lexer_token->sce_token_str.str__, "false")) bool_value = false;

	return create_sce_ast_node(NULL, NULL, &bool_value, sizeof(bool), E_Sce_Ast_Bool_Value);
}

Sce_Ast_Node* create_sce_value_literal_ast(Sce_Lexer_Token* sce_lexer_token) {
	int err_code = 0;
	bool is_float = false;
	for (uint8_t* buf = sce_lexer_token->sce_token_str.str__; *buf; ++buf) if (*buf == '.') is_float = true;
	if (is_float) {
		double d = strtold(sce_lexer_token->sce_token_str.str__, NULL);
		if (d > FLT_MAX) return  create_sce_ast_node(NULL, NULL, &d, sizeof(double), E_Sce_Ast_Double_Value);
		else {
			float f = (float)d;
			return  create_sce_ast_node(NULL, NULL, &f, sizeof(float), E_Sce_Ast_Float_Value);
		}
	}
	else {
		int err;
		uint64_t d = (uint64_t)change_str_to_ints(&err, sce_lexer_token->sce_token_str.str__);
		return create_sce_ast_node(NULL, NULL, &d, sizeof(uint64_t), E_Sce_Ast_Int_Value);
	}
}

Sce_Ast_Type get_lexer_expect_consume_sce(Sce_Lexer_Type lexer_type, Sce_Lexer_Token_Lists* sce_lexer_token_lists, const uint8_t** sce_ast_tokes, const Sce_Ast_Type* sce_ast_types, const uint32_t size_) {
	for (uint32_t i = 0; i < size_; ++i) if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, lexer_type, sce_ast_tokes[i])) return sce_ast_types[i];
	return E_Sce_Ast_NULL;
}

Sce_Ast_Type get_expect_consume_sce_mul_div_mod(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	const uint8_t* tokens[] = {
		"*", "/", "%"
	};
	const Sce_Ast_Type ast_type[] = {
		E_Sce_Ast_Mul, E_Sce_Ast_Div, E_Sce_Ast_Mod
	};
	return get_lexer_expect_consume_sce(E_Sce_Lexer_Operator, sce_lexer_token_lists, tokens, ast_type, sizeof(tokens) / sizeof(tokens[0]));
}

Sce_Ast_Type get_expect_consume_sce_add_sub(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	const uint8_t* tokens[] = {
		"+", "-"
	};
	const Sce_Ast_Type ast_type[] = {
		E_Sce_Ast_Add, E_Sce_Ast_Sub
	};
	return get_lexer_expect_consume_sce(E_Sce_Lexer_Operator, sce_lexer_token_lists, tokens, ast_type, sizeof(tokens) / sizeof(tokens[0]));
}

Sce_Ast_Type get_expect_consume_sce_assigment(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	const uint8_t* tokens[] = {
		"=", "+=", "-=", "*=", "/=", "%="
	};
	const Sce_Ast_Type ast_type[] = {
		E_Sce_Ast_Assigment, E_Sce_Ast_Add_Assigment, E_Sce_Ast_Sub_Assigment, E_Sce_Ast_Mul_Assigment, E_Sce_Ast_Div_Assigment, E_Sce_Ast_Mod_Assigment
	};
	return get_lexer_expect_consume_sce(E_Sce_Lexer_Operator, sce_lexer_token_lists, tokens, ast_type, sizeof(tokens) / sizeof(tokens[0]));
}

Sce_Ast_Type get_expect_consume_sce_shifts(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	const uint8_t* tokens[] = {
		"<<", ">>"
	};
	const Sce_Ast_Type ast_type[] = {
		E_Sce_Ast_Left_Shift, E_Sce_Ast_Right_Shift
	};
	return get_lexer_expect_consume_sce(E_Sce_Lexer_Operator, sce_lexer_token_lists, tokens, ast_type, sizeof(tokens) / sizeof(tokens[0]));
}
Sce_Ast_Type get_expect_consume_sce_equals(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	const uint8_t* tokens[] = {
		"==", "!="
	};
	const Sce_Ast_Type ast_type[] = {
		E_Sce_Ast_Equal, E_Sce_Ast_Not_Equal
	};
	return get_lexer_expect_consume_sce(E_Sce_Lexer_Operator, sce_lexer_token_lists, tokens, ast_type, sizeof(tokens) / sizeof(tokens[0]));
}

Sce_Ast_Type get_expect_consume_sce_relatives(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	const uint8_t* tokens[] = {
		"<", ">", "<=", ">="
	};
	const Sce_Ast_Type ast_type[] = {
		E_Sce_Ast_Less, E_Sce_Ast_Grater, E_Sce_Ast_Less_Equal, E_Sce_Ast_Grater_Equal
	};
	return get_lexer_expect_consume_sce(E_Sce_Lexer_Operator, sce_lexer_token_lists, tokens, ast_type, sizeof(tokens) / sizeof(tokens[0]));
}

Sce_Ast_Node* start_build_sce_ast(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	return build_sce_ast_module(sce_lexer_token_lists);
}
Sce_Ast_Node* build_sce_ast_module(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	sce_lexer_token_lists->token_size = sce_lexer_token_lists->token_index;

	sce_lexer_token_lists->token_index = 0;
	Sce_Ast_Node* block_ = smart_malloc(Sce_Ast_Node, 1);
	Sce_Ast_Node* dst_ = block_;
	if (!block_) return NULL;
	block_->data.ast_type = E_Sce_Ast_Global_Modules_Statement;
	block_->data.buf = NULL;
	block_->left = NULL;
	block_->right = NULL;
	/*
	* NOW
	* NEW         OLD
	* NODE--------NODE----
	*   |          |____
	*  |___IMP
	* 
	* 
	*  OLD         NEW
	* NODE--------NODE----
	*   |          |____
	*  |___IMP
	*/
	Sce_Ast_Node* filed_expr_ = NULL;
	Sce_Ast_Node* filed_expr_tail_ = NULL;
	while (sce_lexer_token_lists->token_index < sce_lexer_token_lists->token_size) {
		if (block_->right) {

			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) break;
			Sce_Ast_Node* right_ = build_sce_ast_toplevel_statement(sce_lexer_token_lists);

			if (right_ && is_sce_ast_type(right_, E_Sce_Ast_Expr)) {
				if (!filed_expr_tail_) {
					filed_expr_tail_ = left_;
					filed_expr_tail_->left = NULL;
					filed_expr_tail_->right = right_;
					filed_expr_tail_->data.ast_type = E_Sce_Ast_Global_Modules_Statement;
					filed_expr_tail_->data.buf = NULL;
					filed_expr_ = filed_expr_tail_;
				}
				else {
					left_->data.ast_type = E_Sce_Ast_Global_Modules_Statement;
					left_->data.buf = NULL;
					left_->right = right_;
					left_->left = NULL;
					filed_expr_tail_->left = left_;
					filed_expr_tail_ = left_;
				}
				continue;
			}
			left_->left = NULL;
			left_->right = right_;
			left_->data.buf = NULL;
			left_->data.ast_type = E_Sce_Ast_Global_Modules_Statement;
			block_->left = left_;
			block_ = left_;
		}
		else {
			block_->right = build_sce_ast_toplevel_statement(sce_lexer_token_lists);

		}
	}
	if (filed_expr_) {
		if (dst_->right && is_sce_ast_type(dst_->right, E_Sce_Ast_Expr)) {
			Sce_Ast_Node* now_ = dst_;
			dst_ = dst_->left;
			now_->left = filed_expr_;
			filed_expr_ = now_;

		}
		filed_expr_tail_->left = dst_;
		dst_ = filed_expr_;
	}
	return dst_;
}

int get_end_block_process(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (sce_lexer_token_lists->token_index >= sce_lexer_token_lists->token_size) return -1;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "}")) return 0;
	return 1;
}
Sce_Ast_Node* build_sce_ast_expr_statement(Sce_Ast_Node* ast, Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
	if (!left_) return NULL;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ";")) {
		left_->left = ast;
		left_->right = NULL;
		set_up_sce_ast_node(left_, NULL, 0, E_Sce_Ast_Expr);
	}
	else {
		left_->left = ast;
		left_->right = NULL;
		set_up_sce_ast_node(left_, "error : expect to ';' after expr.", sizeof("error : expect to ';' after expr."), E_Sce_Ast_Error);
		consume_sce_token_lists(sce_lexer_token_lists);

	}
	return left_;
}
Sce_Ast_Node* build_sce_ast_toplevel_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_;
	if ((left_ = build_ast_sce_block(sce_lexer_token_lists))) return left_;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ";"))	return build_sce_ast_null_statement(sce_lexer_token_lists);
	if ((left_ = build_sce_ast_toplevel_keyword(sce_lexer_token_lists))) return left_;
	consume_sce_token_lists(sce_lexer_token_lists);

	return NULL;
}

Sce_Ast_Node* build_sce_ast_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_;
	if ((left_ = build_ast_sce_block(sce_lexer_token_lists))) return left_;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ";"))	return build_sce_ast_null_statement(sce_lexer_token_lists);
	if ((left_ = build_sce_ast_keyword(sce_lexer_token_lists))) return left_;
	left_ = build_sce_ast_expr_conmma(sce_lexer_token_lists);
	return build_sce_ast_expr_statement(left_, sce_lexer_token_lists);
}

Sce_Ast_Node* build_sce_ast_base_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_;
	if ((left_ = build_ast_sce_block(sce_lexer_token_lists))) return left_;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ";"))	return build_sce_ast_null_statement(sce_lexer_token_lists);
	if ((left_ = build_sce_ast_base_keyword(sce_lexer_token_lists))) return left_;
	if ((left_ = build_sce_ast_base_if(sce_lexer_token_lists))) return left_;
	if ((left_ = build_sce_ast_return(sce_lexer_token_lists))) return left_;
	left_ = build_sce_ast_expr_conmma(sce_lexer_token_lists);
	return build_sce_ast_expr_statement(left_, sce_lexer_token_lists);
}
Sce_Ast_Node* build_ast_sce_block(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	return NULL;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
	}
	return NULL;
}
Sce_Ast_Node* build_sce_ast_null_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left = smart_malloc(Sce_Ast_Node, 1);
	if (!left) return NULL;
	left->data.buf = left->left = left->right = NULL;
	left->data.ast_type = E_Sce_Ast_Expr;
	return left;
}

#define SET_ERROR_NODE(node, err_literal)\
set_up_sce_ast_node(node,err_literal, sizeof(err_literal), E_Sce_Ast_Error);
Sce_Ast_Node* build_sce_ast_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
	if (!right_) return NULL;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
		right_->data.ast_type = E_Sce_Ast_Else;
		right_->left = NULL;
		right_->data.buf = NULL;

		int error_code;
		Sce_Ast_Node* block_ = build_sce_ast_block_process(&error_code, sce_lexer_token_lists, E_Sce_Ast_Else_Section);
		right_->right = block_;
		if (error_code == 0) return right_;
		else {
			Sce_Ast_Node* rleft_ = smart_malloc(Sce_Ast_Node, 1);
			if (!rleft_) return NULL;
			rleft_->left = NULL;
			rleft_->right = right_;
			SET_ERROR_NODE(rleft_, "error : extpect to '}' after block.");
			return rleft_;
		}
	}
	else {
		right_->right = build_sce_ast_expr_conmma(sce_lexer_token_lists);
		right_->left = NULL;
		right_->data.ast_type = E_Sce_Ast_Else;
		right_->data.buf = NULL;
		return build_sce_ast_expr_statement(right_, sce_lexer_token_lists);
	}
}
Sce_Ast_Node* build_sce_ast_return(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "return")) {
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ";")) {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) return NULL;
			left_->left = left_->right = NULL;
			left_->data.buf = NULL;
			left_->data.ast_type = E_Sce_Ast_Return;
			return left_;
		}
		else {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) return NULL;
			left_->left = build_sce_ast_expr_conmma(sce_lexer_token_lists);
			left_->data.buf = NULL;
			left_->data.ast_type = E_Sce_Ast_Return;
			build_sce_ast_expr_statement(left_, sce_lexer_token_lists);
			return left_;

		}
	}
	return NULL;
}
Sce_Ast_Node* build_sce_ast_if_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type if_else_type) {
	Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
	if (!right_) return NULL;

	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "(")) {
		Sce_Ast_Node* left_ = build_sce_ast_expr_conmma(sce_lexer_token_lists);
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ")")) {
			if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
				right_->data.ast_type = if_else_type;
				right_->left = left_;
				right_->data.buf = NULL;

				int error_code;
				Sce_Ast_Node* block_ = build_sce_ast_block_process(&error_code, sce_lexer_token_lists, if_else_type == E_Sce_Ast_If ? E_Sce_Ast_If_Section : E_Sce_Ast_Else_If_Section);
				right_->right = block_;
				if (error_code == 0) return right_;
				else {
					Sce_Ast_Node* rleft_ = smart_malloc(Sce_Ast_Node, 1);
					if (!rleft_) return NULL;
					rleft_->left = NULL;
					rleft_->right = right_;
					SET_ERROR_NODE(rleft_, "error : extpect to '}' after block.");
					return rleft_;
				}
			}
			else {
				right_->right = build_sce_ast_expr_conmma(sce_lexer_token_lists);
				right_->left = left_;
				right_->data.ast_type = if_else_type;
				right_->data.buf = NULL;
				return build_sce_ast_expr_statement(right_, sce_lexer_token_lists);
			}
		}
		else {
			right_->left = NULL;
			right_->right = left_;
			SET_ERROR_NODE(right_, "error : extpect to ')' after if condition.");
			return right_;
		}
	}
	SET_ERROR_NODE(right_, "error : extpect to '(' after if.");
	right_->left = NULL;
	right_->right = NULL;
	return right_;
}

Sce_Ast_Node* build_sce_ast_base_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
	if (!right_) return NULL;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
		right_->data.ast_type = E_Sce_Ast_Else;
		right_->left = NULL;
		right_->data.buf = NULL;

		int error_code;
		Sce_Ast_Node* block_ = build_sce_ast_base_block_process(&error_code, sce_lexer_token_lists, E_Sce_Ast_Else_Section);
		right_->right = block_;
		if (error_code == 0) return right_;
		else {
			Sce_Ast_Node* rleft_ = smart_malloc(Sce_Ast_Node, 1);
			if (!rleft_) return NULL;
			rleft_->left = NULL;
			rleft_->right = right_;
			SET_ERROR_NODE(rleft_, "error : extpect to '}' after block.");
			return rleft_;
		}
	}
	else {
		right_->right = build_sce_ast_expr_conmma(sce_lexer_token_lists);
		right_->left = NULL;
		right_->data.ast_type = E_Sce_Ast_Else;
		right_->data.buf = NULL;
		return build_sce_ast_expr_statement(right_, sce_lexer_token_lists);
	}
}
Sce_Ast_Node* build_sce_ast_base_if_else(Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type if_else_type) {
	Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
	if (!right_) return NULL;

	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "(")) {
		Sce_Ast_Node* left_ = build_sce_ast_expr_conmma(sce_lexer_token_lists);
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ")")) {
			if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
				right_->data.ast_type = if_else_type;
				right_->left = left_;
				right_->data.buf = NULL;

				int error_code;
				Sce_Ast_Node* block_ = build_sce_ast_base_block_process(&error_code, sce_lexer_token_lists, if_else_type == E_Sce_Ast_If ? E_Sce_Ast_If_Section : E_Sce_Ast_Else_If_Section);
				right_->right = block_;
				if (error_code == 0) return right_;
				else {
					Sce_Ast_Node* rleft_ = smart_malloc(Sce_Ast_Node, 1);
					if (!rleft_) return NULL;
					rleft_->left = NULL;
					rleft_->right = right_;
					SET_ERROR_NODE(rleft_, "error : extpect to '}' after block.");
					return rleft_;
				}
			}
			else {
				right_->right = build_sce_ast_expr_conmma(sce_lexer_token_lists);
				right_->left = left_;
				right_->data.ast_type = if_else_type;
				right_->data.buf = NULL;
				return build_sce_ast_expr_statement(right_, sce_lexer_token_lists);
			}
		}
		else {
			right_->left = NULL;
			right_->right = left_;
			SET_ERROR_NODE(right_, "error : extpect to ')' after if condition.");
			return right_;
		}
	}
	SET_ERROR_NODE(right_, "error : extpect to '(' after if.");
	right_->left = NULL;
	right_->right = NULL;
	return right_;
}

Sce_Ast_Node* build_sce_ast_if(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "if")) {
		return build_sce_ast_if_else(sce_lexer_token_lists, E_Sce_Ast_If);
	}
	else if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "else")) {
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "if")) return build_sce_ast_if_else(sce_lexer_token_lists, E_Sce_Ast_Else_If); 
		return build_sce_ast_else (sce_lexer_token_lists);
	}
	return NULL;
}

Sce_Ast_Node* build_sce_ast_base_if(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "if")) {
		return build_sce_ast_base_if_else(sce_lexer_token_lists, E_Sce_Ast_If);
	}
	else if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "else")) {
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "if")) return build_sce_ast_if_else(sce_lexer_token_lists, E_Sce_Ast_Else_If);
		return build_sce_ast_base_else(sce_lexer_token_lists);
	}
	return NULL;
}

Sce_Ast_Node* build_sce_ast_base_keyword(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "obj")) {
		return build_sce_ast_iden_decl(sce_lexer_token_lists);
	}
	
	return NULL;
}


Sce_Ast_Node* build_sce_ast_keyword(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_;
	if ((left_ = build_sce_ast_base_keyword(sce_lexer_token_lists))) return left_;
	if (left_ = build_sce_ast_if(sce_lexer_token_lists)) return left_;
	if ((left_ = build_sce_ast_call_section_statement(sce_lexer_token_lists))) return left_;

	return NULL;
}

Sce_Ast_Node* build_sce_ast_name_meta(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	/**
	* name event_exp : repeat = 1
	* or
	* name event_exp : repeat
	* or
	* name event_exp : once
	* 
	*/
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Operator, ":")) {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_)return NULL;
		left_->left = left_->right = NULL;
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "repeat")) {
			left_->data.ast_type = E_Sce_Ast_Name_Repeat;

			if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Operator, "=")) {
				Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
				if (!right_) return left_;
				set_up_sce_ast_node(right_, NULL, 0, E_Sce_Ast_Assigment);
				right_->left = left_;
				right_->right = build_sce_ast_expr_conmma(sce_lexer_token_lists);
				left_ = right_;
			}

		}
		else if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "once")) {
			left_->data.ast_type = E_Sce_Ast_Name_Once;
		}
		else {

			SET_ERROR_NODE(left_, "error : expect 'repeat' or 'once' after ':'.");
		}
		return left_;

	}
	return NULL;
}

Sce_Ast_Node* build_sce_ast_toplevel_keyword(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "obj")) {
		return build_sce_ast_iden_decl(sce_lexer_token_lists);
	}
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "name")) {
		Sce_Lexer_Token* sce_lexer_token = peek_sce_token_lists(sce_lexer_token_lists);
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_) return NULL;
		set_up_sce_ast_node(left_, sce_lexer_token->sce_token_str.str__, sce_lexer_token->sce_token_str.index__ + 1, E_Sce_Ast_Name_Statement_Decl);

		consume_sce_token_lists(sce_lexer_token_lists);
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
			int error_code = 0;
			Sce_Ast_Node* block_ = build_sce_ast_name_statement(&error_code, sce_lexer_token_lists);
			left_->right = block_;
			left_->left = NULL;
			if (error_code != 0) {
				Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
				if (!right_) return left_;
				set_up_sce_ast_node(right_, "error : expect to '}' after block.", sizeof("error : expect to '}' after block."), E_Sce_Ast_Error);
				right_->left = NULL;
				right_->right = left_;
			}
			return left_;
		}
		else {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) return NULL;
			left_->left = left_->right = NULL;
			set_up_sce_ast_node(left_, "error : expect to '{' after section decl.", sizeof("error : expect to '{' after section decl."), E_Sce_Ast_Error);
		}
	}
	return NULL;
}
Sce_Ast_Node* build_sce_ast_name_statement(int* error_code, Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* block_ = smart_malloc(Sce_Ast_Node, 1);
	Sce_Ast_Node* dst_ = block_;
	if (!block_) return NULL;
	block_->data.ast_type = E_Sce_Ast_Name_Statement;
	block_->data.buf = NULL;
	block_->left = NULL;
	block_->right = NULL;
	uint8_t has_section = 0x0;

	Sce_Ast_Node* filed_expr_ = NULL;
	Sce_Ast_Node* filed_expr_tail_ = NULL;
	while ((*error_code = get_end_block_process(sce_lexer_token_lists)) > 0) {
		Sce_Ast_Node* right_ = NULL;
		if (block_->right) {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) break;
			right_ = build_sce_ast_name_field_statement(sce_lexer_token_lists);
			if (right_ && is_sce_ast_type(right_, E_Sce_Ast_Expr)) {
				if (!filed_expr_tail_) {
					filed_expr_tail_ = left_;
					filed_expr_tail_->left = NULL;
					filed_expr_tail_->right = right_;
					filed_expr_tail_->data.ast_type = E_Sce_Ast_Name_Statement;
					filed_expr_tail_->data.buf = NULL;
					filed_expr_ = filed_expr_tail_;
				}
				else {
					left_->data.ast_type = E_Sce_Ast_Name_Statement;
					left_->data.buf = NULL;
					left_->right = right_;
					left_->left = NULL;
					filed_expr_tail_->left = left_;
					filed_expr_tail_ = left_;
				}
			}
			else {
				left_->left = NULL;
				left_->right = right_;

				left_->data.buf = NULL;
				left_->data.ast_type = E_Sce_Ast_Name_Statement;
				block_->left = left_;
				block_ = left_;
			}
		}
		else {
			block_->right = build_sce_ast_name_field_statement(sce_lexer_token_lists);
			right_ = block_->right;
		}
		if (right_) {
			if (right_->data.ast_type == E_Sce_Ast_Start_Section_Decl) {
				if (has_section & (1 << 0)) {
					set_up_sce_ast_node(block_, "error : redefined start section.", sizeof("error : redefined start section."), E_Sce_Ast_Error);
					break;
				}
				has_section |= 1 << 0;
			}
			else if (right_->data.ast_type == E_Sce_Ast_End_Section_Decl) {
				if (has_section & (1 << 1)) {

					set_up_sce_ast_node(block_, "error : redefined end section.", sizeof("error : redefined end section."), E_Sce_Ast_Error);
					break;
				}

				has_section |= 1 << 1;
			}
			else if (right_->data.ast_type == E_Sce_Ast_Main_Section_Decl) {
				if (has_section & (1 << 2)) {

					set_up_sce_ast_node(block_, "error : redefined main section.", sizeof("error : redefined main section."), E_Sce_Ast_Error);
					break;
				}

				has_section |= 1 << 2;
			}
		}
	}
	if (filed_expr_) {
		if (dst_->right && is_sce_ast_type(dst_->right, E_Sce_Ast_Expr)) {
			Sce_Ast_Node* now_ = dst_;
			dst_ = dst_->left;
			now_->left = filed_expr_;
			filed_expr_ = now_;

		}
		filed_expr_tail_->left = dst_;
		dst_ = filed_expr_;
	}

	return dst_;
}
Sce_Ast_Node* build_sce_ast_name_field_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "obj")) return build_sce_ast_iden_decl(sce_lexer_token_lists);
	Sce_Ast_Node* left_ = build_sce_ast_section_statement(sce_lexer_token_lists);

	if (left_) return left_;
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ";"))	return build_sce_ast_null_statement(sce_lexer_token_lists);
	return NULL;
}
Sce_Ast_Node* build_sce_ast_call_section_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "exit")) return build_sce_ast_call_section(E_Sce_Ast_Exit_Call_Section, sce_lexer_token_lists);
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "start")) return build_sce_ast_call_section(E_Sce_Ast_Start_Call_Section, sce_lexer_token_lists);
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "main")) return build_sce_ast_call_section(E_Sce_Ast_Main_Call_Section, sce_lexer_token_lists);
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "end")) return build_sce_ast_call_section(E_Sce_Ast_End_Call_Section, sce_lexer_token_lists);
	return NULL;
}
Sce_Ast_Node* build_sce_ast_call_section(Sce_Ast_Type sce_ast_call_section_type, Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Operator, ";")) {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_) return NULL;
		left_->left = NULL;
		left_->right = NULL;
		left_->data.ast_type = sce_ast_call_section_type;
		left_->data.buf = NULL;
		return left_;
	}
	else {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_) return NULL;
		left_->left = NULL;
		left_->right = NULL;
		set_up_sce_ast_node(left_, "error : extpect to ';' after block.", sizeof("error : extpect to ';' after block."), E_Sce_Ast_Error);
		return left_;
	}
}

Sce_Ast_Node* build_sce_ast_section_statement(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "start"))
		return build_sce_ast_block(E_Sce_Ast_Start_Section_Decl, sce_lexer_token_lists);
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "end"))
		return build_sce_ast_block(E_Sce_Ast_End_Section_Decl, sce_lexer_token_lists);
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "main"))
		return build_sce_ast_block(E_Sce_Ast_Main_Section_Decl, sce_lexer_token_lists);
	return NULL;
}

Sce_Ast_Node* build_sce_ast_block(Sce_Ast_Type sce_ast_section_type, Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
		int error_code = 0;
		Sce_Ast_Node* block_ = build_sce_ast_block_process(&error_code, sce_lexer_token_lists, sce_ast_section_type);
		if (error_code == 0) return block_;
		else {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) return NULL;
			left_->left = NULL;
			left_->right = block_;
			set_up_sce_ast_node(left_, "error : extpect to '}' after block.", sizeof("error : extpect to '}' after block."), E_Sce_Ast_Error);
			return left_;
		}
	}
	Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
	if (!left_) return NULL;
	left_->left = NULL;
	left_->right = NULL;
	set_up_sce_ast_node(left_, "error : extpect to '{' or ';' after block.", sizeof("error : extpect to '{' or ';' after block."), E_Sce_Ast_Error);
	return left_;
}

Sce_Ast_Node* build_sce_ast_base_block(Sce_Ast_Type sce_ast_section_type, Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
		int error_code = 0;
		Sce_Ast_Node* block_ = build_sce_ast_base_block_process(&error_code, sce_lexer_token_lists, sce_ast_section_type);
		if (error_code == 0) return block_;
		else {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) return NULL;
			left_->left = NULL;
			left_->right = block_;
			set_up_sce_ast_node(left_, "error : extpect to '}' after block.", sizeof("error : extpect to '}' after block."), E_Sce_Ast_Error);
			return left_;
		}
	}
	Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
	if (!left_) return NULL;
	left_->left = NULL;
	left_->right = NULL;
	set_up_sce_ast_node(left_, "error : extpect to '{' or ';' after block.", sizeof("error : extpect to '{' or ';' after block."), E_Sce_Ast_Error);
	return left_;
}
Sce_Ast_Node* build_sce_ast_block_process(int* error_code, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type sce_ast_block_type) {
	Sce_Ast_Node* block_ = smart_malloc(Sce_Ast_Node, 1);
	Sce_Ast_Node* dst_ = block_;
	if (!block_) return NULL;
	block_->data.ast_type = sce_ast_block_type;
	block_->data.buf = NULL;
	block_->left = NULL;
	block_->right = NULL;
	while ((*error_code = get_end_block_process(sce_lexer_token_lists)) > 0) {
		if (block_->right) {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) break;
			Sce_Ast_Node* right_ = build_sce_ast_statement(sce_lexer_token_lists);
			left_->left = NULL;
			left_->right = right_;

			left_->data.buf = NULL;
			left_->data.ast_type = sce_ast_block_type;
			block_->left = left_;
			block_ = left_;
		}
		else {
			block_->right = build_sce_ast_statement(sce_lexer_token_lists);
		}
	}
	return dst_;
}

Sce_Ast_Node* build_sce_ast_base_block_process(int* error_code, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Ast_Type sce_ast_block_type) {
	Sce_Ast_Node* block_ = smart_malloc(Sce_Ast_Node, 1);
	Sce_Ast_Node* dst_ = block_;

	if (!block_) return NULL;
	block_->data.ast_type = sce_ast_block_type;
	block_->data.buf = NULL;
	block_->left = NULL;
	block_->right = NULL;
	while ((*error_code = get_end_block_process(sce_lexer_token_lists)) > 0) {
		if (block_->right) {
			Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
			if (!left_) break;
			Sce_Ast_Node* right_ = build_sce_ast_base_keyword(sce_lexer_token_lists);
			left_->left = NULL;
			left_->right = right_;

			left_->data.buf = NULL;
			left_->data.ast_type = sce_ast_block_type;
			block_->left = left_;
			block_ = left_;
		}
		else {
			block_->right = build_sce_ast_base_keyword(sce_lexer_token_lists);
		}
	}
	return dst_;
}

Sce_Ast_Node* build_sce_ast_function_args(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
	if (!left_)return NULL;
	left_->left = left_->right = NULL;
	set_up_sce_ast_node(left_, NULL, 0, E_Sce_Ast_Parameter_Args_Decl);
	do {
		if (!expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, "obj")) break;
		Sce_Lexer_Token* sce_lexer_token = peek_sce_token_lists(sce_lexer_token_lists);
		if (!sce_lexer_token)break;

		if (sce_lexer_token->sce_token_type == E_Sce_Lexer_Iden) {
			Sce_Ast_Node* right_;
			right_ = smart_malloc(Sce_Ast_Node, 1);
			if (!right_) break;
			right_->left = left_;
			right_->right = NULL;
			set_up_sce_ast_node(right_, NULL, 0, E_Sce_Ast_Parameter_Args_Decl);
			left_ = right_;
		}
		else {
			Sce_Ast_Node* right_;
			right_ = smart_malloc(Sce_Ast_Node, 1);
			if (!right_) break;
			right_->left = left_;
			right_->right = NULL;
			set_up_sce_ast_node(right_, "error : expect to iden after parameter obj.", sizeof("error : expect to iden after parameter obj."), E_Sce_Ast_Error);
			left_ = right_;
			break;
		}
	} while (!expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Keyword, ","));

	return left_;
}

Sce_Ast_Node* build_sce_ast_iden_decl(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_;
	Sce_Lexer_Token* sce_lexer_token = peek_sce_token_lists(sce_lexer_token_lists);
	if (!sce_lexer_token) return NULL;
	left_ = smart_malloc(Sce_Ast_Node, 1);
	if (!left_) return NULL;

	if (sce_lexer_token->sce_token_type == E_Sce_Lexer_Iden) {
		left_->data.buf = (uint8_t*)dupalloc(sce_lexer_token->sce_token_str.str__, sizeof(uint8_t) * (sce_lexer_token->sce_token_str.index__ + 1));
		consume_sce_token_lists(sce_lexer_token_lists);

		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Operator, "=")) {
			left_->left = left_->right = NULL;
			left_->data.ast_type = E_Sce_Ast_Var_Decl;
			Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
			if (!right_) return left_;
			set_up_sce_ast_node(right_, NULL, 0, E_Sce_Ast_Assigment);
			right_->left = left_;
			right_->right = build_sce_ast_expr_conmma(sce_lexer_token_lists);

			left_ = right_;
		}
		else if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "(")) {
			if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, ")")) {
				left_->left = left_->right = NULL;
			}
			else left_->right = build_sce_ast_function_args(sce_lexer_token_lists);
			left_->data.ast_type = E_Sce_Ast_Function_Decl;
			if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
				int error_code = 0;
				Sce_Ast_Node* block_ = build_sce_ast_base_block_process(&error_code, sce_lexer_token_lists, E_Sce_Ast_Modules_Statement);
				if (error_code == 0) {
					left_->left = block_;
					return left_;
				}
				else {
					Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
					if (!right_) return NULL;
					right_->left = left_;
					right_->right = block_;
					set_up_sce_ast_node(right_, "error : extpect to '}' after block.", sizeof("error : extpect to '}' after block."), E_Sce_Ast_Error);
					return right_;
				}
			}
			Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
			if (!right_) return NULL;
			right_->left = left_;
			right_->right = NULL;
			set_up_sce_ast_node(right_, "error : extpect to '{' or ';' after block.", sizeof("error : extpect to '{' or ';' after block."), E_Sce_Ast_Error);
			return right_;
		}
		else {
			left_->left = left_->right = NULL;

			left_->data.ast_type = E_Sce_Ast_Var_Decl;
		}
	}
	else {
		set_up_sce_ast_node(left_, NULL, 0, E_Sce_Ast_NULL);
		left_->left = left_->right = NULL;
	}
	return build_sce_ast_expr_statement(left_, sce_lexer_token_lists);
}

Sce_Ast_Node* build_sce_ast_instance_obj_hepler(Sce_Lexer_Token_Lists* sce_lexer_token_lists) {
	Sce_Ast_Node* left_;
	Sce_Lexer_Token* sce_lexer_token = peek_sce_token_lists(sce_lexer_token_lists);
	if (!sce_lexer_token) return NULL;
	left_ = smart_malloc(Sce_Ast_Node, 1);
	if (!left_) return NULL;

	if (sce_lexer_token->sce_token_type == E_Sce_Lexer_Iden) {
		left_->data.buf = (uint8_t*)dupalloc(sce_lexer_token->sce_token_str.str__, sizeof(uint8_t) * (sce_lexer_token->sce_token_str.index__ + 1));
		consume_sce_token_lists(sce_lexer_token_lists);
		
		left_->data.ast_type = E_Sce_Ast_Instance_Obj;
		if (expect_advance_consume_sce_token_lists(sce_lexer_token_lists, E_Sce_Lexer_Punchcute, "{")) {
			int error_code = 0;
			Sce_Ast_Node* block_ = build_sce_ast_base_block_process(&error_code, sce_lexer_token_lists, E_Sce_Ast_Instance_Parameter);
			if (error_code == 0) {
				left_->left = block_;
				return left_;
			}
			else {
				Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
				if (!right_) return NULL;
				right_->left = left_;
				right_->right = block_;
				set_up_sce_ast_node(right_, "error : extpect to '}' after block.", sizeof("error : extpect to '}' after block."), E_Sce_Ast_Error);
				return right_;
			}
		}
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_) return NULL;
		right_->left = left_;
		right_->right = NULL;
		set_up_sce_ast_node(right_, "error : extpect to '{' after obj isnatnce symbol.", sizeof("error : extpect to '{' after obj isnatnce symbol."), E_Sce_Ast_Error);
		return right_;
	}
	return NULL;
}
Sce_Ast_Node* build_sce_ast_instance_obj(Sce_Lexer_Token_Lists* src_ans_token_list) {
	/*
	* DECL VAR
	* OBJ I = OBJ I;
	* VAR
	* I = OBJ I;
	* 
	*/
	if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Keyword, "obj")) {
		Sce_Ast_Node* block_ = smart_malloc(Sce_Ast_Node, 1);
		if (!block_) return NULL;

		Sce_Ast_Node* right_ = build_sce_ast_instance_obj_hepler(src_ans_token_list);
		block_->right = right_;
		block_->left = NULL;
		block_->data.ast_type = E_Sce_Ast_Instance_Obj;
		block_->data.buf = NULL;
		return block_;
	}
	return NULL;
}
Sce_Ast_Node* build_sce_ast_expr_conmma(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_assigment(src_ans_token_list);
	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, ",")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = E_Sce_Ast_Expr_Comma;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_assigment(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_assigment(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_bit_or(src_ans_token_list);
	Sce_Ast_Type tmp_;
	while ((tmp_ = get_expect_consume_sce_assigment(src_ans_token_list))) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = tmp_;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_assigment(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_bit_or(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_bit_xor(src_ans_token_list);
	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, "|")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = E_Sce_Ast_Bit_Or;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_bit_xor(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_bit_xor(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_bit_and(src_ans_token_list);
	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, "^")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = E_Sce_Ast_Bit_Xor;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_bit_and(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_bit_and(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_equals(src_ans_token_list);
	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, "&")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = E_Sce_Ast_Bit_And;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_equals(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_equals(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_relatives(src_ans_token_list);
	Sce_Ast_Type tmp_;
	while (tmp_ = get_expect_consume_sce_equals(src_ans_token_list)) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = tmp_;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_relatives(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_relatives(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_shifts(src_ans_token_list);
	Sce_Ast_Type tmp_;
	while (tmp_ = get_expect_consume_sce_relatives(src_ans_token_list)) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = tmp_;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_shifts(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_shifts(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_add_sub(src_ans_token_list);
	Sce_Ast_Type tmp_;
	while (tmp_ = get_expect_consume_sce_shifts(src_ans_token_list)) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = tmp_;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_add_sub(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_add_sub(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_mul_div_mod(src_ans_token_list);
	Sce_Ast_Type tmp_;
	while (tmp_ = get_expect_consume_sce_add_sub(src_ans_token_list)) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = tmp_;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_mul_div_mod(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_mul_div_mod(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_sign_symbol(src_ans_token_list);
	Sce_Ast_Type tmp_;
	while (tmp_ = get_expect_consume_sce_mul_div_mod(src_ans_token_list)) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = tmp_;
		right_->data.buf = NULL;
		right_->right = build_sce_ast_sign_symbol(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_sign_symbol(Sce_Lexer_Token_Lists* src_ans_token_list) {
	if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, "-")) {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_)return NULL;
		Sce_Ast_Node* right_ = build_sce_ast_sign_symbol(src_ans_token_list);
		left_->data.ast_type = E_Sce_Ast_Sub;
		left_->data.buf = NULL;
		left_->right = right_;
		return left_;
	}
	else if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, "~")) {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_)return NULL;
		Sce_Ast_Node* right_ = build_sce_ast_sign_symbol(src_ans_token_list);
		left_->data.ast_type = E_Sce_Ast_Bit_Not;
		left_->right = right_;
		return left_;
	}
	else if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, "1")) {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_)return NULL;
		Sce_Ast_Node* right_ = build_sce_ast_sign_symbol(src_ans_token_list);
		left_->data.ast_type = E_Sce_Ast_Bit_Not_Logic;
		left_->right = right_;
		return left_;
	}
	else return build_sce_ast_memebers(src_ans_token_list);
}
Sce_Ast_Node* build_sce_ast_memebers(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_call_functions(src_ans_token_list);
	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, ".")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = E_Sce_Ast_Member_Acess;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_call_functions(src_ans_token_list);
		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_args_functions(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_assigment(src_ans_token_list);
	if (!left_) return NULL;

	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Operator, ",")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		right_->data.ast_type = E_Sce_Ast_Args;
		right_->data.buf = NULL;

		right_->right = build_sce_ast_assigment(src_ans_token_list);

		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_call_functions(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* left_ = build_sce_ast_pare(src_ans_token_list);
	while (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Punchcute, "(")) {
		Sce_Ast_Node* right_ = smart_malloc(Sce_Ast_Node, 1);
		if (!right_)break;
		right_->left = left_;
		if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Punchcute, ")")) {
			right_->right = NULL;
			right_->data.ast_type = E_Sce_Ast_Call_Function;
			right_->data.buf = NULL;
		}
		else {
			right_->right = build_sce_ast_args_functions(src_ans_token_list);
			if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Punchcute, ")")) {
				right_->data.ast_type = E_Sce_Ast_Call_Function;
				right_->data.buf = NULL;
			}
			else set_up_sce_ast_node(
				right_, "error ; expect to ')' after call functions.", sizeof("error ; expect to ')' after call functions."), E_Sce_Ast_Error);
		}

		left_ = right_;
	}
	return left_;
}

Sce_Ast_Node* build_sce_ast_pare(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Ast_Node* inst_ = build_sce_ast_instance_obj(src_ans_token_list);
	if (inst_) return inst_;
	if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Punchcute, "(")) {
		Sce_Ast_Node* left_ = smart_malloc(Sce_Ast_Node, 1);
		if (!left_) return NULL;
		left_->right = build_sce_ast_expr_conmma(src_ans_token_list);
		if (expect_advance_consume_sce_token_lists(src_ans_token_list, E_Sce_Lexer_Punchcute, ")")) set_up_sce_ast_node(left_, NULL, 0, E_Sce_Ast_Pare);
		else set_up_sce_ast_node(left_, "error : expect to ')'", sizeof("error : expect to ')'"), E_Sce_Ast_Error);
		return left_;
	}
	else return build_sce_ast_basic_token(src_ans_token_list);
}

Sce_Ast_Node* build_sce_ast_basic_token(Sce_Lexer_Token_Lists* src_ans_token_list) {
	Sce_Lexer_Token* now_token = peek_sce_token_lists(src_ans_token_list);
	Sce_Ast_Node* left_;
	if (!now_token) return NULL;
	switch (now_token->sce_token_type) {
	case E_Sce_Lexer_Iden:
		//printf("%s\n", now_token->sce_token_str.str__);
		left_ = create_sce_ast_node(NULL, NULL, now_token->sce_token_str.str__, now_token->sce_token_str.index__ + 1, E_Sce_Ast_Iden);
		break;
	case E_Sce_Lexer_Value_Literal:
		left_ = create_sce_value_literal_ast(now_token);
		break;
	case E_Sce_Lexer_Bool_Literal:
		left_ = create_sce_bool_literal_ast(now_token);
		break;
		
	case E_Sce_Lexer_Str_Literal:
		left_ = create_sce_ast_node(NULL, NULL, now_token->sce_token_str.str__, now_token->sce_token_str.index__ + 1, E_Sce_Ast_String_Value);
		break;
	case E_Sce_Lexer_Keyword:
		left_ = create_sce_ast_node(NULL, NULL, "error : expect to operator.", sizeof("error : expect to operator."), E_Sce_Ast_Error);
		break;
	default:

		left_ = create_sce_ast_node(NULL, NULL, "error : expect to operator.", sizeof("error : expect to operator."), E_Sce_Ast_Error);
	}
	consume_sce_token_lists(src_ans_token_list);
	return left_;
}