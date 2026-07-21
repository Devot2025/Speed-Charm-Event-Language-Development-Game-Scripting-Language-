#pragma once
#include "std_ext/ext_strs.h"
#define simple_strcmp(str1, str2) strcmp(str1, str2) == 0

static inline bool is_u8byte_digit(const uint8_t byte__) {
	if (byte__ >= '0' && byte__ <= '9') return true;
	return false;
}
static inline bool u8byte_str__(const uint8_t byte__, const uint8_t * str__) {
	while (*str__) if (*str__++ == byte__) return true;
	return false;
}
static inline bool u8strs_str__(const uint8_t** byte__, const uint8_t* str__) {
	while (*byte__) if (simple_strcmp(*byte__++, str__)) return true;
	return false;
}
static inline bool is_u8byte_utf8words(uint8_t byte__) {

	/* ASCII */
	if ((byte__ >= 'A' && byte__ <= 'Z') ||
		(byte__ >= 'a' && byte__ <= 'z')) {
		return true;
	}

	/* UTF-8 start byte */
	if ((byte__ & 0x80) != 0) {
		return true;
	}

	return false;
}

static inline bool is_utf8_string(const uint8_t* sce_token) {
	if (!sce_token || !*sce_token) return false;

	while (*sce_token) {
		if (*sce_token <= 0x7F) {
			sce_token++;
		}
		else if ((*sce_token & 0xE0) == 0xC0) {
			if ((sce_token[1] & 0xC0) != 0x80) return false;
			sce_token += 2;
		}
		else if ((*sce_token & 0xF0) == 0xE0) {
			if ((sce_token[1] & 0xC0) != 0x80) return false;
			if ((sce_token[2] & 0xC0) != 0x80) return false;
			sce_token += 3;
		}
		else if ((*sce_token & 0xF8) == 0xF0) {
			if ((sce_token[1] & 0xC0) != 0x80) return false;
			if ((sce_token[2] & 0xC0) != 0x80) return false;
			if ((sce_token[3] & 0xC0) != 0x80) return false;
			sce_token += 4;
		}
		else {
			return false;
		}
	}
	return true;
}
#define u8strlen_u32(s) (uint32_t) strlen((const char *)s)

typedef struct U8_String_Buffers {
	uint32_t index__;
	uint32_t size__;
	uint8_t* str__;
}U8_String_Buffers;
#define U8_STR_REQUIRE_DEFAULT_SIZE 10
static inline void init_u8_string_buffers(U8_String_Buffers * u8_string_buffers, uint32_t size__){
	if (size__ < U8_STR_REQUIRE_DEFAULT_SIZE) size__ = U8_STR_REQUIRE_DEFAULT_SIZE;
	u8_string_buffers->index__ = 0;
	u8_string_buffers->size__ = size__;
	u8_string_buffers->str__ = smart_malloc(uint8_t, size__);
	if (!u8_string_buffers->str__) return;
	*(u8_string_buffers->str__) = '\0';
}

static inline void delete_u8_string_buffers(U8_String_Buffers * u8_string_buffers) {
	u8_string_buffers->index__ = 0;
	u8_string_buffers->size__ = 0;
	s_free(u8_string_buffers->str__);
}
static inline void insert_u8_byte_u8_string_buffer(U8_String_Buffers* u8_string_buffers, uint8_t byte__) {
	*(u8_string_buffers->str__ + u8_string_buffers->index__) = byte__;
}
static inline void append_u8_byte_u8_string_basic(U8_String_Buffers* u8_string_buffers, const uint8_t* str__, size_t buf_size) {

	size_t tmp_size = u8_string_buffers->size__;
	if (!safety_realloc(
		(void**)&u8_string_buffers->str__,
		u8_string_buffers->index__ + buf_size + 1,
		&tmp_size,
		sizeof(uint8_t))) return;
	u8_string_buffers->size__ = (uint32_t)tmp_size;
	memcpy(u8_string_buffers->str__ + u8_string_buffers->index__, str__, sizeof(uint8_t) * buf_size);
	u8_string_buffers->index__ += (uint32_t)buf_size;
	insert_u8_byte_u8_string_buffer(u8_string_buffers, '\0');
}
static inline void append_u8_byte_u8_string(U8_String_Buffers* u8_string_buffers, const uint8_t *str__) {
	/**
	* 0 1 2 3
	* 'A' 'B 'C'
	* len = 3
	* 
	*/
	uint32_t len_ = u8strlen_u32(str__);
	size_t tmp_size = u8_string_buffers->size__;
	if (!safety_realloc(
		(void**)&u8_string_buffers->str__,
		u8_string_buffers->index__ + len_ + 1,
		&tmp_size,
		sizeof(uint8_t))) return;
	u8_string_buffers->size__ = (uint32_t)tmp_size;
	memcpy(u8_string_buffers->str__ + u8_string_buffers->index__, str__, sizeof(uint8_t) * len_);
	u8_string_buffers->index__ += len_;
	insert_u8_byte_u8_string_buffer(u8_string_buffers, '\0');	
}
static inline void append_u8_byte_u8_string_buffer(U8_String_Buffers* u8_string_buffers, uint8_t byte__) {
	size_t tmp_size = u8_string_buffers->size__;
	if(!safety_realloc((void **) & u8_string_buffers->str__, u8_string_buffers->index__ + 2, &tmp_size, sizeof(uint8_t))) return;
	u8_string_buffers->size__ = (uint32_t)tmp_size;
	insert_u8_byte_u8_string_buffer(u8_string_buffers, byte__);
	*(u8_string_buffers->str__ + u8_string_buffers->index__++ + 1) = '\0';
}

static inline void append_u8_string_buffer_u8_string_buffer(U8_String_Buffers* u8_string_buffers, const U8_String_Buffers* u8_string_buffers_1) {
	size_t tmp_size = u8_string_buffers->size__;

	if (!safety_realloc(
		(void**)&u8_string_buffers->str__,
		u8_string_buffers->index__ + u8_string_buffers_1->index__ + 1,
		&tmp_size,
		sizeof(uint8_t))) return;
	u8_string_buffers->size__ = (uint32_t)tmp_size;

	memcpy(
		u8_string_buffers->str__ + u8_string_buffers->index__,
		u8_string_buffers_1->str__,
		sizeof(uint8_t) *
		u8_string_buffers_1->index__
	);
	u8_string_buffers->index__ += u8_string_buffers_1->index__;
	insert_u8_byte_u8_string_buffer(
		u8_string_buffers, '\0'
	);
}
static inline void empty_u8_byte_u8_string_bufferr(U8_String_Buffers* u8_string_buffers) {
	*(u8_string_buffers->str__) = '\0';
	u8_string_buffers->index__ = 0;
}

static inline void empty_from_pos_u8_byte_u8_string_bufferr(U8_String_Buffers* u8_string_buffers, uint32_t pos__) {
	if (pos__ > u8_string_buffers->index__) return;
	u8_string_buffers->index__ = pos__;
	u8_string_buffers->str__[pos__] = '\0';
}

static inline U8_String_Buffers move_u8_byte_u8_string_bufferr(U8_String_Buffers* u8_string_buffers, uint32_t size__) {

	if (u8_string_buffers->index__ < size__) size__ = u8_string_buffers->index__;
	uint8_t* buf = (uint8_t*)dupalloc(u8_string_buffers->str__, size__ * sizeof(uint8_t) + 1);
	
	empty_u8_byte_u8_string_bufferr(u8_string_buffers);
	buf[size__] = '\0';
	return (U8_String_Buffers){
			size__,
			size__ + 1,
			buf
	};
}
#include <inttypes.h>
#include <stdarg.h>

static inline uint8_t* multidysnprinf(uint32_t* size_, uint8_t * format_,...) {
	va_list va_;
	va_list va__;
	va_start(va_, format_);
	va_copy(va__, va_);
	int len_ = vsnprintf(NULL, 0, (char*)format_, va_);
	if (len_ < 0) {
		va_end(va_);
		va_end(va__);
		return NULL;
	}
	len_ += 1;
	*size_ = len_;
	uint8_t* buf_ = smart_malloc(uint8_t, *size_);
	(void)vsnprintf((char*)buf_, len_, (char*)format_, va__);
	va_end(va_);
	va_end(va__);
	return buf_;
}
static inline void u8strbuf_dysnprinf(U8_String_Buffers * dst, uint8_t* format_, ...) {
	va_list va_;
	va_list va__;
	va_start(va_, format_);
	va_copy(va__, va_);
	int len_ = vsnprintf(NULL, 0, (char*)format_, va_);
	if (len_ < 0) {
		va_end(va_);
		va_end(va__);
		return;
	}
	size_t size_ = dst->size__;
	if (!safety_realloc((void**)&dst->str__, dst->index__ + len_ + 1, &size_, sizeof(uint8_t))) {
		va_end(va_);
		va_end(va__);
		return;
	}
	dst->size__ = (uint32_t)size_;
	(void)vsnprintf((char*)dst->str__ + dst->index__, len_ + 1, (char*)format_, va__);
	dst->index__ += (uint32_t)len_;

	va_end(va_);
	va_end(va__);

}

#define to_u16_str(v, s)\
	multidysnprinf(s, "%" PRIu16, v);

#define to_u32_str(v, s)\
	multidysnprinf("%" PRIu32, v);

#define to_u64_str(v, s)\
	multidysnprinf(s, "%" PRIu64, v);

#define to_16_str(v, s)\
	multidysnprinf(s, "%" PRIi16, v);

#define to_32_str(v, s)\
	multidysnprinf(s, "%" PRIi32, v);

#define to_64_str(v, s)\
	multidysnprinf(s, "%" PRIi64, v);

#define to_float_str(v, s)\
	multidysnprinf(s, "%f", v);
#define to_double_str(v, s)\
	multidysnprinf(s, "%lf", v);
#define to_u16_u8strbuf(dst, v)\
	u8strbuf_dysnprinf(dst, "%" PRIu16, v)
#define to_u32_u8strbuf(dst, v)\
	u8strbuf_dysnprinf(dst, "%" PRIu32, v)
#define to_u64_u8strbuf(dst, v)\
	u8strbuf_dysnprinf(dst, "%" PRIu64, v)
#define to_float_u8strbuf(dst, v)\
	u8strbuf_dysnprinf(dst, "%f", v)
#define to_double_u8strbuf(dst, v)\
	u8strbuf_dysnprinf(dst, "%lf", v)
