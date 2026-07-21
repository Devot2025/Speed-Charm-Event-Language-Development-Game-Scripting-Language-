#include "sce_lexer.h"

//typedef struct Token_
#define SCE_TOKENS_ARRAY(...) {__VA_ARGS__, NULL}

void init_lexer_sce_lists(Sce_Lexer_Token_Lists* sce_token_lists, uint32_t size__) {
	if (size__ < SCE_REQUIRE_DEFAULT_SIZE) size__ = SCE_REQUIRE_DEFAULT_SIZE;
	sce_token_lists->tokens = smart_malloc(Sce_Lexer_Token, size__);
	if (!sce_token_lists->tokens) return;
	sce_token_lists->token_size = size__;
	sce_token_lists->token_index = 0;
}

void delete_lexer_sce_lists(Sce_Lexer_Token_Lists* sce_token_lists) {
	for (uint32_t i = 0; i < sce_token_lists->token_index; ++i) {
		delete_u8_string_buffers(&(*(sce_token_lists->tokens + i)).sce_token_str);
	}
	s_free(sce_token_lists->tokens);
	sce_token_lists->token_index = 0;
	sce_token_lists->token_size = 0;
}

static bool is_sce_token_literals(const uint8_t* sce_token) {
	const uint8_t* sce_literal_tokens[] =
		SCE_TOKENS_ARRAY("false", "true");
	const uint8_t** sce_literal_token = sce_literal_tokens;
	while (*sce_literal_token) if (simple_strcmp(sce_token, *sce_literal_token++)) return true;
	return false;
}

bool is_sce_token_keyword(const uint8_t* sce_token) {

	const uint8_t* sce_keyword_tokens[]
		= {
		"while", "if", "else", "name", "next", "start", "main", "end", "exit", "once", "repeat",
		"bool", "char", "int", "float", "str", "vec2", "vec3", "vec4", "quat", "obj", 
		NULL
	};
	const uint8_t** sce_keyword_token = sce_keyword_tokens;
	while (*sce_keyword_token) if (simple_strcmp(sce_token, *sce_keyword_token++)) return true;
	return false;
}


int is_sce_token_value(const uint8_t* sce_token) {
	bool is_float = false;
	if (!is_u8byte_digit(*sce_token)) {
		/*NOT DIGIST*/
		if (*sce_token == '.') is_float = true;
		else return 0;
	}

	while (*sce_token) {
		if (*sce_token == '.') {
			/*find to dot in binary.*/
			/**/
			if (is_float)return -1;
			is_float = true;
		}
		else if (!is_u8byte_digit(*sce_token)) {
			return -1;
		}
		sce_token++;
	}
	return 1;
}

Sce_Lexer_Type get_lexer_token_type(const uint8_t* sce_token) {
	if (is_sce_token_keyword(sce_token)) return E_Sce_Lexer_Keyword;
	int err__ = is_sce_token_value(sce_token);
	if (err__ == -1)return E_Sce_Lexer_Error_Value_Literal;
	if (err__ == 1)return E_Sce_Lexer_Value_Literal;
	if (is_sce_token_literals(sce_token)) return E_Sce_Lexer_Bool_Literal;
	return E_Sce_Lexer_Iden;
}

void append_lexer_token_lists(Sce_Lexer_Token_Lists* sce_token_lists, U8_String_Buffers* stack_buffers, Sce_Lexer_Type sce_lexer_type) {
	if (stack_buffers->index__ == 0)return;
	size_t tmp_size = sce_token_lists->token_size;
	if (!safety_realloc(&sce_token_lists->tokens, sce_token_lists->token_index, &tmp_size, sizeof(Sce_Lexer_Token))) return;
	sce_token_lists->token_size = (uint32_t)tmp_size;

	(*(sce_token_lists->tokens + sce_token_lists->token_index)).sce_token_str.str__ = (uint8_t*)dupalloc(stack_buffers->str__, stack_buffers->size__ * sizeof(uint8_t));
	(*(sce_token_lists->tokens + sce_token_lists->token_index)).sce_token_str.index__ = stack_buffers->index__;
	(*(sce_token_lists->tokens + sce_token_lists->token_index)).sce_token_str.size__ = stack_buffers->size__;
	//printf("%d", sce_token_lists->token_index);
	if (sce_lexer_type == E_Sce_Lexer_NULL) sce_lexer_type = get_lexer_token_type(stack_buffers->str__);
	(*(sce_token_lists->tokens + sce_token_lists->token_index++)).sce_token_type = sce_lexer_type;
	empty_u8_byte_u8_string_bufferr(stack_buffers);
}
Sce_Lexer_Token_Lists start_lexer_sce(const uint8_t* sce_code) {
	Sce_Lexer_Mode now_mode = E_Sce_Lexer_Normal_Mode;
	U8_String_Buffers stack_buffers;
	Sce_Lexer_Token_Lists sce_lexer_token_lists;
	init_u8_string_buffers(&stack_buffers, 20);
	init_lexer_sce_lists(&sce_lexer_token_lists, 20);
	while (*sce_code) {
		uint8_t byte_ = *sce_code;

		if (now_mode == E_Sce_Lexer_Normal_Mode) split_normal_sce_token(byte_, &sce_lexer_token_lists, &now_mode, &stack_buffers);
		else if (now_mode == E_Sce_Lexer_Operator_Mode) { if (!split_operator_sce_token(byte_, &sce_lexer_token_lists, &now_mode, &stack_buffers))continue; }
		else if (now_mode == E_Sce_Lexer_Dot_Mode) { if (split_dot_sce_token(byte_, &sce_lexer_token_lists, &now_mode, &stack_buffers)) continue; }
		else if (now_mode == E_Sce_Lexer_Start_String_Mode) { split_start_string_sce_token(byte_, &now_mode, &stack_buffers); }
		else if (now_mode == E_Sce_Lexer_Next_String_Mode) { if (split_next_string_sce_token(byte_, &sce_lexer_token_lists, &now_mode, &stack_buffers))continue; }
		else if (now_mode == E_Sce_Lexer_Doll_String_Mode) split_doll_string_sce_token(byte_, &now_mode, &stack_buffers);
		else if (now_mode == E_Sce_Comment_Mode) split_comment_sce_token(byte_, &now_mode);
		else if (now_mode == E_Sce_Line_Comment_Mode) split_line_comment_sce_token(byte_, &now_mode);
		else if (now_mode == E_Sce_Multi_Line_Comment_Start_Mode) split_multi_line_comment_sce_token(byte_, &now_mode);
		else if (now_mode == E_Sce_Multi_Line_Comment_End_Mode) split_end_multi_line_comment_sce_token(byte_, &now_mode);
		else {
			tmp_printf_e("Error Log : Lexer Mode %d is Invalid ", now_mode);
			assert(false);
		}
		sce_code++;

	}

	append_lexer_token_lists(&sce_lexer_token_lists, &stack_buffers, E_Sce_Lexer_NULL);

	delete_u8_string_buffers(&stack_buffers);
	return sce_lexer_token_lists;
}

void split_normal_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (u8byte_str__(sce_byte__, ";(){}")) {
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
		append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_Punchcute);
	}
	else if (u8byte_str__(sce_byte__, "=+-*%/&|^~<>")) {
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
		append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
		*now_mode__ = E_Sce_Lexer_Operator_Mode;
	}
	else if (u8byte_str__(sce_byte__, ",![]?:")) {
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
		append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_Operator);
	}
	else if (sce_byte__ == '\"') {
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
		*now_mode__ = E_Sce_Lexer_Start_String_Mode;
	}
	else if (sce_byte__ == '\'') {
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
		*now_mode__ = E_Sce_Lexer_Start_Char_Mode;
	}
	else if (sce_byte__ == '.') *now_mode__ = E_Sce_Lexer_Dot_Mode;
	else if (sce_byte__ == '@') *now_mode__ = E_Sce_Comment_Mode;
	else if (u8byte_str__(sce_byte__, " \n\r\b	")) append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
	else append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
}

bool split_operator_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	const uint8_t* two_byte_operator_[] = {
		"==", "!=", "+=", "-=", "*=","/=", "%=", "<=", ">=", "&&", "||",
		"^=", "~=", "|=", "&=", "++", "--", "<<", ">>", "::", NULL
	};

	append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
	bool is_two_byte__ = u8strs_str__(two_byte_operator_, stack_buffers->str__);
	*now_mode__ = E_Sce_Lexer_Normal_Mode;
	U8_String_Buffers tmp_str_buffer = move_u8_byte_u8_string_bufferr(stack_buffers, is_two_byte__ ? 2 : 1);
	//empty_u8_byte_u8_string_bufferr(stack_buffers);
	append_lexer_token_lists(sce_lexer_token_lists, &tmp_str_buffer, E_Sce_Lexer_Operator);
	delete_u8_string_buffers(&tmp_str_buffer);
	return is_two_byte__;
}
bool split_dot_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (is_u8byte_utf8words(sce_byte__)) {
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_NULL);
		append_u8_byte_u8_string_buffer(stack_buffers, '.');
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_Operator);
	}
	else {
		append_u8_byte_u8_string_buffer(stack_buffers, '.');
	}
	*now_mode__ = E_Sce_Lexer_Normal_Mode;
	return true;
}
void split_char_sce_token(const uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (sce_byte__ == '\\') *now_mode__ = E_Sce_Lexer_Doll_Char_Mode;
	else if (sce_byte__ == '\'') *now_mode__ = E_Sce_Lexer_Normal_Mode;
	//else if (sce_byte__ == '\n') *now_mode__ = E_Sce_Lexer_Normal_Mode;
	else {
		*now_mode__ = E_Sce_Lexer_Next_Char_Mode;
		append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
	}
}

void split_next_sce_token(const uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (sce_byte__ == '\'') {
		if (stack_buffers->index__ == 0)append_u8_byte_u8_string_buffer(stack_buffers, '\0');
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_Str_Literal);
		*now_mode__ = E_Sce_Lexer_Normal_Mode;
	}
	else {
		
	}
}

void split_start_string_sce_token(const uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (sce_byte__ == '\\') *now_mode__ = E_Sce_Lexer_Doll_String_Mode;
	else if (sce_byte__ == '\"') *now_mode__ = E_Sce_Lexer_Next_String_Mode;
	//else if (sce_byte__ == '\n') *now_mode__ = E_Sce_Lexer_Normal_Mode;
	else append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
}

void split_doll_string_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (sce_byte__ == '\\') sce_byte__ = '\\';
	else if (sce_byte__ == 'n') sce_byte__ = '\n';
	else if (sce_byte__ == 'b') sce_byte__ = '\b';
	else if (sce_byte__ == 'r') sce_byte__ = '\r';
	else if (sce_byte__ == 't') sce_byte__ = '\t';
	else if (sce_byte__ == '"') sce_byte__ = '\"';
	append_u8_byte_u8_string_buffer(stack_buffers, sce_byte__);
	*now_mode__ = E_Sce_Lexer_Start_String_Mode;
}
bool split_next_string_sce_token(uint8_t sce_byte__, Sce_Lexer_Token_Lists* sce_lexer_token_lists, Sce_Lexer_Mode* now_mode__, U8_String_Buffers* stack_buffers) {
	if (sce_byte__ == '\"') *now_mode__ = E_Sce_Lexer_Start_String_Mode;
	else if (sce_byte__ != '\n') {
		if (stack_buffers->index__ == 0)append_u8_byte_u8_string_buffer(stack_buffers, '\0');
		append_lexer_token_lists(sce_lexer_token_lists, stack_buffers, E_Sce_Lexer_Str_Literal);
		*now_mode__ = E_Sce_Lexer_Normal_Mode;
		return true;
	}
	return false;
}
void split_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__) {
	if (sce_byte__ == ':') *now_mode__ = E_Sce_Multi_Line_Comment_Start_Mode;
	else *now_mode__ = E_Sce_Line_Comment_Mode;
}
void split_line_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__) {
	if (sce_byte__ == '\n') *now_mode__ = E_Sce_Lexer_Normal_Mode;
}

void split_multi_line_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__) {
	if (sce_byte__ == ':') *now_mode__ = E_Sce_Multi_Line_Comment_End_Mode;
}

void split_end_multi_line_comment_sce_token(uint8_t sce_byte__, Sce_Lexer_Mode* now_mode__) {
	if (sce_byte__ == '@') *now_mode__ = E_Sce_Lexer_Normal_Mode;
	else *now_mode__ = E_Sce_Multi_Line_Comment_Start_Mode;
}