#include "sce_lexer.h"
#include "sce_parser.h"
#include "sce_ast_debug.h"
#include "sce_ir.h"
#include "sce_ir_debug.h"
#include "sce_run.h"

uint8_t* load_files(const uint8_t* paths);
uint8_t* insert_std_code(uint8_t* buf);

int main(void) {

	uint8_t* buf = load_files("test.sce");
	buf = insert_std_code(buf);
	Sce_Lexer_Token_Lists sce_lex = start_lexer_sce(buf);
	for (uint32_t i = 0; i < sce_lex.token_index; ++i) {
		printf("%u %s\n", i, sce_lex.tokens[i].sce_token_str.str__);
	}

	Sce_Ast_Node* sce_ast = start_build_sce_ast(&sce_lex);
	s_free(buf);
	start_print_ast_debug(sce_ast);
	delete_lexer_sce_lists(&sce_lex);
	bool ir_is_error;
	Sce_Binary_Machine_Instructions* ir = sce_start_build_binary_run_code(&ir_is_error, sce_ast);
	delete_sce_ast_node(sce_ast);
	if (ir_is_error) return -1;
	print_sce_ir(ir, false);
	Sce_Run_VM_Context vm_context = start_sce_vm_context(ir);

	putchar('\n');
	putchar('\n');
	printf("SCE FILE NAME test.sce\n");
	printf("-------------------------------------------------------\n");
	printf("-------------------------------------------------------\n");
	putchar('\n');
	run_sce_vm_context(&vm_context, (const uint8_t *)"EVENT1");
	putchar('\n');
	putchar('\n');

	printf("-------------------------------------------------------\n");
	printf("-------------------------------------------------------\n");
	delete_sce_run_vm_context(&vm_context);
	//Sce_Binary_Machine_Instructions* sc_i = sce_start_build_binary_run_code(sce_ast);

	return 0;
}

uint8_t* load_files(const uint8_t* paths) {

	if (!paths) return NULL;
	FILE* f__ = fopen(paths, "rb");
	if (!f__)return NULL;
	size_t size_ = 0;
	fseek(f__, 0, SEEK_END);
	size_ = ftell(f__);
	rewind(f__);
	uint8_t* buf = smart_malloc(uint8_t, size_ + 1);
	if (!buf) {
		fclose(f__);
		return NULL;
	}
	fread(buf, sizeof(uint8_t), size_, f__);
	buf[size_] = '\0';
	fclose(f__);
	return buf;
}

uint8_t* insert_std_code(uint8_t* buf) {
	const char std_lists[] =
		"obj stdout  = __open__(0);\n"
		"obj stdin   = __open__(1);\n"
		"obj stderr  = __open__(2);\n";
	size_t len_ =
		safety_strlen(buf);
	uint8_t* new_ = smart_realloc(uint8_t, buf, len_ + sizeof(std_lists)
	);
	if (!new_) return buf;
	memcpy(new_ + len_, std_lists, sizeof(std_lists));
	return new_;
}