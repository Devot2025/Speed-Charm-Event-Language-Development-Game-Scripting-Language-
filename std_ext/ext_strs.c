#include "ext_strs.h"


char * ext_str_cpy(char * dst, const char * src, size_t dst_len, size_t src_len){
    if(!dst) return NULL;
    if(!src) return NULL;
    /**
     * dst src
     * 10 5
     * 5
     * dst src
     * 5 10
     * 5
    */
    size_t cpy_len__ = dst_len < src_len ? dst_len : src_len;
    smart_memcpy_p(dst, src,  char, cpy_len__);
    return dst;
}
bool check_digit_char(const char word){
    return word >= ET_CHAR_1 && word <= ET_CHAR_9;
}
char * change_int_to_str(int value, size_t * buffer_size){
    int len = snprintf(NULL, 0, "%d", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%d", value);
    return buffer;
}
char * change_long_int_to_str(long int value, size_t * buffer_size){
    int len = snprintf(NULL, 0, "%ld", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%ld", value);
    return buffer;
}

char * change_long_long_int_to_str(long long int value, size_t * buffer_size){
    int len = snprintf(NULL, 0, "%lld", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%lld", value);
    return buffer;
}

char * change_u_long_int_to_str(unsigned int value, size_t *buffer_size){
    int len = snprintf(NULL, 0, "%u", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%u", value);
    return buffer;
}

char * change_u_long_long_int_to_str(unsigned long int value, size_t *buffer_size){
    int len = snprintf(NULL, 0, "%lu", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%lu", value);
    return buffer;
}

char * change_u_int_to_str(unsigned long long int value, size_t *buffer_size){
    int len = snprintf(NULL, 0, "%llu", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%llu", value);
    return buffer;
}
char * change_float_to_str(float value, size_t *buffer_size){
    int len = snprintf(NULL, 0, "%f", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%f", value);
    return buffer;
}

char * change_double_to_str(double value, size_t *buffer_size){
    int len = snprintf(NULL, 0, "%lf", value);
    if(len < 0)return NULL;
    char * buffer = smart_calloc(char, len + 1);
    if(!buffer) return NULL;
    *buffer_size = (size_t)len + 1;
    snprintf(buffer, *buffer_size, "%lf", value);
    return buffer;
}

size_t change_str_to_ints(int * error_code, const char * str){
    size_t result = 0;
    if(!*str){
        *error_code = NONE_STR_ERROR_CODE;
        return 0;
    }
    while (*str) {

        if(*str < '0' || *str > '9'){
            *error_code = INVALID_STR_ERROR_CODE;
            return 0;
        }
        result = result * 10 + (size_t)(*str - '0');
        str++;
    }
    return result;
}

int change_str_to_int(int * error_code, const char * str){
    int minus = 1;
    if(*str == '-'){
        str++;
        minus = -1;
    }
    size_t result = change_str_to_ints(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE)return 0;
    if(minus == 1){
        if(result > (size_t)INT_MAX){
            *error_code = RANGE_ERROR_CODE;
            return 0;
        }
    }
    else{
        if(result > (size_t)INT_MAX + 1){
            *error_code = RANGE_ERROR_CODE;
            return 0;
        }
        if(result == (size_t)INT_MAX + 1){
            *error_code = 0;
            return INT_MIN;
        }
    }
    *error_code = VALID_CHANGE_OUT_CODE;
    return minus * (int)result;
}


long int change_str_to_long_int(int * error_code, const char * str){
    int minus = 1;
    if(*str == '-'){
        str++;
        minus = -1;
    }
    size_t result = change_str_to_ints(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE)return 0;
    if(minus == 1){
        if(result > (size_t)LONG_MAX){
            *error_code = RANGE_ERROR_CODE;
            return 0;
        }
    }
    else{
        if(result > (size_t)LONG_MAX + 1){
            *error_code = RANGE_ERROR_CODE;
            return 0;
        }
        if(result == (size_t)LONG_MAX + 1){
            *error_code = INVALID_STR_ERROR_CODE;
            return LONG_MIN;
        }
    }
    *error_code = INVALID_STR_ERROR_CODE;
    return minus * (long)result;
}

long long int change_str_to_long_long_int(int * error_code, const char * str){
    int minus = 1;
    if(*str == '-'){
        str++;
        minus = -1;
    }
    size_t result = change_str_to_ints(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE)return 0;
    if(minus == 1){
        if(result > (size_t)LLONG_MAX){
            *error_code = RANGE_ERROR_CODE;
            return 0;
        }
    }
    else{
        if(result > (size_t)LLONG_MAX + 1){
            *error_code = RANGE_ERROR_CODE;
            return 0;
        }
        if(result == (size_t)LLONG_MAX + 1){
            *error_code = VALID_CHANGE_OUT_CODE;
            return LLONG_MIN;
        }
    }
    *error_code = VALID_CHANGE_OUT_CODE;
    return minus * (long long)result;
}


unsigned int change_str_to_u_int(int * error_code, const char * str){
    size_t result = change_str_to_ints(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE)return 0;
    if(result > (size_t)UINT_MAX){
        *error_code = RANGE_ERROR_CODE;
        return 0;
    }
    else{
        *error_code = VALID_CHANGE_OUT_CODE;
        return (unsigned int)result;
    }
}

unsigned long int change_str_to_long_u_int(int * error_code, const char * str){
    size_t result = change_str_to_ints(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE)return 0;

    if(result > (size_t)ULONG_MAX){
        *error_code = RANGE_ERROR_CODE;
        return 0;
    }
    else{
        *error_code = VALID_CHANGE_OUT_CODE;
        return (unsigned int)result;
    }
}

unsigned long long int change_str_to_long_long_u_int(int * error_code, const char * str){
    size_t result = change_str_to_ints(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE)return 0;

    if(result > (size_t)ULLONG_MAX){
        *error_code = RANGE_ERROR_CODE;
        return 0;
    }
    else{
        *error_code = VALID_CHANGE_OUT_CODE;
        return (unsigned int)result;
    }
}

void str_float_checker(int * error_code, const char * cache_str, bool * s_float_check){
    bool have_digit = false;
    while(*cache_str){
        if(*cache_str == '.'){
            if(*s_float_check == false) *s_float_check = true;
            else{
                *error_code = DOUBLE_DOT_STR_ERROR_CODE;
                return;
            }
            cache_str++;

            continue;
        } 
        if(*cache_str < '0' || *cache_str > '9'){
            *error_code = INVALID_STR_ERROR_CODE;
            return;
        }
        have_digit = true;
        cache_str++;
    }
    if(have_digit) *error_code = VALID_CHANGE_OUT_CODE;
    else *error_code = INVALID_STR_ERROR_CODE;
}

long double change_str_to_doubles(int * error_code, const char * str){
    bool minus = false;
    const char * cache_str = str;
    if(*str == '-') {
        cache_str++;
        minus = true;
    }
    bool float_check = false;
    str_float_checker(error_code, cache_str, &float_check);
    if(*error_code < VALID_CHANGE_OUT_CODE) return 0.0L;
    long double dbl;
    if(float_check) dbl = strtold(str, NULL);
    else {
        /*floatŒno‚È‚¢•¶Žš—ñ‚©‚Â”’l‚Å‚ ‚é‰Â”\«‚Å‚ ‚ê‚Î®”Œn‚ð‰î‚µfloatŒn‚É‚·‚éB*/
        if(!minus) dbl = (long double) change_str_to_long_long_u_int(error_code, cache_str);
        else dbl = (long double)change_str_to_long_long_int(error_code, str);
        if(*error_code < VALID_CHANGE_OUT_CODE) return 0.0L;
    }
    return dbl;
}

float change_str_to_float(int * error_code, const char * str){
    long double dbl = change_str_to_doubles(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE) return 0.0f;
    long double abs_dbl = (dbl < 0.0L) ? -dbl : dbl;
    if((long double)FLT_MAX >= abs_dbl) {
        *error_code = 0;
        return (float)dbl;
    }
    else{
        *error_code = RANGE_ERROR_CODE;
        return 0.0f;
    }
}

double change_str_to_double(int * error_code, const char * str){
    long double dbl = change_str_to_doubles(error_code, str);
    if(*error_code < VALID_CHANGE_OUT_CODE) return 0.0;
    long double abs_dbl = (dbl < 0.0L) ? -dbl : dbl;
    if((long double)DBL_MAX >= abs_dbl) {
        *error_code = 0;
        return (double)dbl;
    }
    else{
        *error_code = RANGE_ERROR_CODE;
        return 0.0f;
    }
}

uint32_t change_str_ptr_str32(const char ** next_str){
    unsigned char * byte_b = (unsigned char *)*next_str;
    if(*byte_b < 0x80){
        //ascii 1byte
        (*next_str)++;
        return (char)*byte_b;
    }
    else if((*byte_b & 0xE0) == 0xC0){
        /*0xE0 = 1110 0000b 0xC0 = 1100 0000*/
        /**
         * 110x xxxxb
        * 1110 0000b
        * ----------
        * 1100 0000b
        * 1100 0000b
        * ----------
        * true
        */
        uint32_t dst_byte = ((*byte_b++ & 0x1F) << 6) | (*byte_b & 0x3F);
        *next_str += 2;
        return dst_byte;

    }
    else if((*byte_b & 0xF0) == 0xE0){
        uint32_t dst_byte = ((*byte_b++ & 0x0F) << 12) | ((*byte_b++ & 0x3F) << 6) | ((*byte_b & 0x3F));
        *next_str += 3;
        return dst_byte;
    }
    else if((*byte_b & 0xF8) == 0xF0){
        uint32_t dst_byte = ((*byte_b++ & 0x7) << 18) | ((*byte_b++ & 0x3F) << 12) | ((*byte_b++ & 0x3F) << 6) | ((*byte_b & 0x3F));
        *next_str += 4;
        return dst_byte;
    }
    else{
        return 0;
    }
}

uint32_t change_strs_and_idx_str32(const char * next_str, size_t * str_idx){
    unsigned char * byte_b = (unsigned char *)next_str;
    if(*byte_b < 0x80){
        //ascii 1byte
        (*str_idx)++;
        return (char)*byte_b;
    }
    else if((*byte_b & 0xE0) == 0xC0){
        /*0xE0 = 1110 0000b 0xC0 = 1100 0000*/
        /**
         * 110x xxxxb
        * 1110 0000b
        * ----------
        * 1100 0000b
        * 1100 0000b
        * ----------
        * true
        */
        uint32_t dst_byte = ((*byte_b++ & 0x1F) << 6) | (*byte_b & 0x3F);
        (*str_idx) += 2;
        return dst_byte;

    }
    else if((*byte_b & 0xF0) == 0xE0){
        uint32_t dst_byte = ((*byte_b++ & 0x0F) << 12) | ((*byte_b++ & 0x3F) << 6) | ((*byte_b & 0x3F));
        (*str_idx) += 3;
        return dst_byte;
    }
    else if((*byte_b & 0xF8) == 0xF0){
        uint32_t dst_byte = ((*byte_b++ & 0x7) << 18) | ((*byte_b++ & 0x3F) << 12) | ((*byte_b++ & 0x3F) << 6) | ((*byte_b & 0x3F));
        (*str_idx) += 4;
        return dst_byte;
    }
    else{
        return 0;
    }
}

uint32_t uint8str_uint32(uint32_t* advance_idx_, const uint8_t* str_) {
    if (!(*str_ & 0x80)) {
        *advance_idx_ = 1;
        return *str_;
    }
    else if ((*str_ & 0xE0) == 0xC0) {
        if ((*(str_ + 1) & 0xC0) == 0x80) {
            *advance_idx_ = 2;
            return (((*str_ & 0x1F) << 6) | ((*(str_ + 1)) & 0x3F));
        }
    }
    else if ((*str_ & 0xF0) == 0xE0) {

        if ((*(str_ + 1) & 0xC0) == 0x80) {
            if ((*(str_ + 2) & 0xC0) == 0x80) {

                *advance_idx_ = 3;
                return
                    ((*str_ & 0xF) << 12) |
                    (((*(str_ + 1)) & 0x3F) << 6) |
                    ((*(str_ + 2)) & 0x3F);
            }
        }
    }
    else if ((*str_ & 0xF8) == 0xF0) {
        if ((*(str_ + 1) & 0xC0) == 0x80)
            if ((*(str_ + 2) & 0xC0) == 0x80)
                if ((*(str_ + 3) & 0xC0) == 0x80) {
                    *advance_idx_ = 4;
                    return
                        ((*str_ & 0x7) << 18) |
                        ((*(str_ + 1) & 0x3F) << 12) |
                        (((*(str_ + 2)) & 0x3F) << 6) |
                        ((*(str_ + 3)) & 0x3F);
                }
    }
    *advance_idx_ = 0;//error_break
    return 0;
}