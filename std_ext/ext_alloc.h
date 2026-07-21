#ifndef _EXT_ALLOC_H_
#define _EXT_ALLOC_H_
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define tmp_printf_e(...) fprintf(stderr, __VA_ARGS__)
#define MALLOC_ERROR_STRING "error : failed to memory allocation."
#define GET_ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

#define e_free    free
#define e_malloc  malloc
#define e_calloc  calloc
#define e_realloc realloc

#define s_free(ptr) (e_free(ptr), ptr = NULL)
#define calc_type_size(type, size) (sizeof(type) * (size))
#define smart_realloc(type, block, size) (type *)e_realloc(block, calc_type_size(type, size))
//#define easy_malloc(type, size) (type * )e_malloc(calc_type_size(type, size))
//#define easy_calloc(type, size) (type * )e_calloc(size, sizeof(type)) 

#define smart_malloc(type, size) (size) ? (type * )e_malloc(calc_type_size(type, size)) : NULL
#define smart_calloc(type, size) (size) ? (type * )e_calloc(size, sizeof(type)) : NULL
#define smart_memset_0(dst, type, size) memset(dst, 0, sizeof(type) * (size))
#define smart_memcpy(dst, src, type, size) memcpy(&dst, &src, (size) * sizeof(type))
#define smart_memcpy_p(dst, src, type, size) memcpy(dst, src, (size) * sizeof(type))
#define smart_memsame(dst, src, type, size) memcmp(dst, src, sizeof(type) * size) == 0
bool safety_realloc(void** block, size_t now_index, size_t* now_size, size_t type_size);
bool clamp_realloc(void** block, size_t clamp_cap, size_t* now_cap, size_t type_size);

void* dupalloc(const void* src_block, size_t size);

#define smart_dupalloc(type, block) dupalloc(block, sizeof(type))
#endif // !_EXT_ALLOC_H_
