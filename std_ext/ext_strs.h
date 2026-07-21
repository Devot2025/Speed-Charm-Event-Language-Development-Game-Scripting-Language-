#ifndef _EXT_STRS_H_
#define _EXT_STRS_H_
#include <std_ext/ext_alloc.h>
#include <limits.h>
#include <float.h>
#include "macro_inf.hint"
#include "c_header_helper.hint"
#include "ext_c.hint"
_EXTERN_C_BEGIN_

#define VALID_CHANGE_OUT_CODE 0
#define NONE_STR_ERROR_CODE -1
#define INVALID_STR_ERROR_CODE -2
#define RANGE_ERROR_CODE -3
#define DOUBLE_DOT_STR_ERROR_CODE -4

#define STRING_LITERAL_SIZE(str_literal) sizeof(str_literal) - 1
#define equal_string(str_0, str_1, size) (memcmp(str_0, str_1, sizeof(char) * size) == 0)

#define len_of_str(str_literal) (STRING_LITERAL_SIZE(str_literal))
#define safety_strlen(str) str ? strlen(str) : 0 FINAL__

char * ext_str_cpy(char * dst, const char * src, size_t dst_len, size_t src_len);
bool check_digit_char(const char word);
char * change_int_to_str(int value, size_t * buffer_size);
char * change_long_int_to_str(long int value, size_t * buffer_size);
char * change_long_long_int_to_str(long long int value, size_t * buffer_size);
char * change_u_long_int_to_str(unsigned int value, size_t *buffer_size);
char * change_u_long_long_int_to_str(unsigned long int value, size_t *buffer_size);
char * change_u_int_to_str(unsigned long long int value, size_t *buffer_size);
char * change_float_to_str(float value, size_t *buffer_size);
char * change_double_to_str(double value, size_t *buffer_size);
size_t change_str_to_ints(int * error_code, const char * str);
int change_str_to_int(int * error_code, const char * str);
long int change_str_to_long_int(int * error_code, const char * str);
long long int change_str_to_long_long_int(int * error_code, const char * str);
unsigned int change_str_to_u_int(int * error_code, const char * str);
unsigned long int change_str_to_long_u_int(int * error_code, const char * str);
unsigned long long int change_str_to_long_long_u_int(int * error_code, const char * str);
float change_str_to_float(int * error_code, const char * str);
double change_str_to_double(int * error_code, const char * str);
uint32_t change_str_ptr_str32(const char ** next_str);
uint32_t change_strs_and_idx_str32(const char * next_str, size_t * str_idx);

uint32_t uint8str_uint32(uint32_t* advance_idx_, const uint8_t* str_);
_EXTERN_C_END_
#endif