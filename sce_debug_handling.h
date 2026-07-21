#ifndef _SCE_DEBUG_HANDLING_H_
#define _SCE_DEBUG_HANDLING_H_
#include "transfer_test_functions.h"
#include "sce_helper.h"


sce_type_struct(
	Sce_Debuger,
	U8_String_Buffers err_strs;
	uint32_t line;
	uint32_t col;
);

#endif //_SCE_DEBUG_HANDLING_H_