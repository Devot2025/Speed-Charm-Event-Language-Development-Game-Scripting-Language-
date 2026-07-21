#ifndef SCE_HELPER_H_
#define SCE_HELPER_H_

#define sce_prefix_symbol(name_) sce_##name_
#define define_func__(block) do{block}while(0)
#define gen_error_string(dst, format, ...)\
	define_func__(\
		uint32_t req = snprintf(NULL, 0, "error : "format, __VA_ARGS__);\
		req += 1;\
		dst =\
		smart_malloc(uint8_t, req);\
		if (!dst) break;\
		(void)snprintf(dst, req, "error : "format, __VA_ARGS__); \
	)

#define type_struct typedef struct
#define sce_type_struct(name, impl)\
	type_struct name{\
		impl\
	}name

#define SCE_BUFFER_127 127


#endif //SCE_HELPER_H_