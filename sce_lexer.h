#ifndef _SCE_LEXER_H_
#define _SCE_LEXER_H_

#include "transfer_test_functions.h"
#include <assert.h>
#define sce_si_strap static inline
typedef enum Sce_Lexer_Mode {
	E_Sce_Lexer_Normal_Mode,
	E_Sce_Lexer_Operator_Mode,
	E_Sce_Lexer_Dot_Mode,
	E_Sce_Lexer_Start_Char_Mode,
	E_Sce_Lexer_Doll_Char_Mode,
	E_Sce_Lexer_Next_Char_Mode,
	E_Sce_Lexer_Start_String_Mode,
	E_Sce_Lexer_Next_String_Mode,
	E_Sce_Lexer_Doll_String_Mode,
	E_Sce_Comment_Mode,
	E_Sce_Line_Comment_Mode,
	E_Sce_Multi_Line_Comment_Start_Mode,
	E_Sce_Multi_Line_Comment_End_Mode,

}Sce_Lexer_Mode;
typedef enum Sce_Lexer_Type {
	E_Sce_Lexer_NULL,
	E_Sce_Lexer_Iden,
	E_Sce_Lexer_Keyword,
	E_Sce_Lexer_Punchcute,
	E_Sce_Lexer_Operator,
	E_Sce_Lexer_Char_Literal,
	E_Sce_Lexer_Str_Literal,
	E_Sce_Lexer_Bool_Literal,
	E_Sce_Lexer_Value_Literal,
	E_Sce_Lexer_Error_Value_Literal,
}Sce_Lexer_Type;
typedef struct Sce_Lexer_Token {
	U8_String_Buffers sce_token_str;
	Sce_Lexer_Type sce_token_type;
}Sce_Lexer_Token;
typedef struct Sce_Lexer_Token_Lists {
	uint32_t token_size;
	uint32_t token_index;
	Sce_Lexer_Token* tokens;
}Sce_Lexer_Token_Lists;
#define SCE_REQUIRE_DEFAULT_SIZE U8_STR_REQUIRE_DEFAULT_SIZE

sce_si_strap Sce_Lexer_Token* peek_sce_token_lists(Sce_Lexer_Token_Lists* sce_token_lists) {
	if (sce_token_lists->token_index >= sce_token_lists->token_size) return NULL;
	if (!sce_token_lists->tokens) return NULL;
	return (sce_token_lists->tokens + sce_token_lists->token_index);
}

sce_si_strap Sce_Lexer_Token* consume_sce_token_lists(Sce_Lexer_Token_Lists* sce_token_lists) {
	if (sce_token_lists->token_index >= sce_token_lists->token_size) return NULL;
	if (!sce_token_lists->tokens) return NULL;
	Sce_Lexer_Token* tmp_now_token = (sce_token_lists->tokens + sce_token_lists->token_index);
	sce_token_lists->token_index++;
	s_free(tmp_now_token->sce_token_str.str__);
	return tmp_now_token;
}

sce_si_strap bool expect_advance_consume_sce_token_lists(Sce_Lexer_Token_Lists* sce_token_lists, Sce_Lexer_Type lexer_type, const uint8_t * sce_lexer_token) {
	if (sce_token_lists->token_index >= sce_token_lists->token_size) return false;
	if (!sce_token_lists->tokens) return false;
	Sce_Lexer_Token * tmp_now_token = (sce_token_lists->tokens + sce_token_lists->token_index);
	if (simple_strcmp(tmp_now_token->sce_token_str.str__, sce_lexer_token) && tmp_now_token->sce_token_type == lexer_type) {
		sce_token_lists->token_index++;
		s_free(tmp_now_token->sce_token_str.str__);
		return true;
	}
	return false;
}

void init_lexer_sce_lists(Sce_Lexer_Token_Lists* sce_token_lists, uint32_t size__);
void delete_lexer_sce_lists(Sce_Lexer_Token_Lists* sce_token_lists);
bool is_sce_token_keyword(const uint8_t* sce_token);
int is_sce_token_value(const uint8_t* sce_token);
Sce_Lexer_Type get_lexer_token_type(const uint8_t* sce_token);
void append_lexer_token_lists(Sce_Lexer_Token_Lists* sce_token_lists, U8_String_Buffers* stack_buffers, Sce_Lexer_Type sce_lexer_type);
Sce_Lexer_Token_Lists start_lexer_sce(const uint8_t* sce_code);

void split_normal_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers);
bool split_operator_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers);
bool split_dot_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers);

void split_start_string_sce_token(const uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers);
void split_doll_string_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers);
bool split_next_string_sce_token(uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers);

void split_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__);
void split_line_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__);
void split_multi_line_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__);
void split_end_multi_line_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__);
#endif // !_SCE_LEXER_H_
