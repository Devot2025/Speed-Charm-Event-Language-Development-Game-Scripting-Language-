#ifndef _SCE_IR_DEBUG_H_
#define _SCE_IR_DEBUG_H_
#include "sce_ir.h"
#define GET_PRINT_STR_SVR(idx) "SVR%lu ", idx
static inline void print_sce_ir(Sce_Binary_Machine_Instructions * sce_bm_inst, bool break_point) {
	if (!sce_bm_inst) return;

	for (Sce_Binary_Machine_Instructions* next_ = sce_bm_inst; next_; next_ = next_->next) {
		printf("_%s ", GET_SCE_IR_STRING(next_));
		if (next_->sce_bmr_code == E_SCE_BINARY_INST_ADD__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SUB__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_MUL__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_DIV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_MOD__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_EQ__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));

			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_NEQ__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));

			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_LS__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));

			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_GT__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));

			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));

		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_GTEQ__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));

			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));

		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_LSEQ__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));

			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_XOR__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_OR__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_AND__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_NOT__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SHL__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SHR__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_NAME__) {
			printf("%s ", next_->scope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_LABEL__) {
			printf("%s ", next_->scope2);
		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SYSTEM_CALL__) {
		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_MOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("&%s ", next_->scope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_MMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("%s ", next_->scope2);

		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_IMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("%u ", next_->icope2);

		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_BMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("%s ", next_->bcope2 ? "true" : "false");
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_FMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("%f ", next_->fcope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_CMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("%c ", next_->ccope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf("%s ", next_->scope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_RMOV__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
			printf(GET_PRINT_STR_SVR(next_->ope2));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_ALLOC__) {
			printf("%s ", next_->scope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_CALL__) {
			printf("%s ", next_->scope2);
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_JMP__) {
			printf(GET_PRINT_STR_SVR(SCE_VIRTUAL_REGISTER_CMP_RETURN));
			printf("%s ", next_->scope2);

		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SAVE_REGISTER_STACK__) {
			printf("%u ", next_->ope1);

		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_LOAD_REGISTER_STACK__) {
			printf("%u ", next_->ope1);

		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_LOAD_REGISTER__) {
			printf("%u ", next_->ope1);
			printf("%u ", next_->ope2);

		}
			
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_SAVE_REGISTER__) {
			printf("%u ", next_->ope1);
			printf("%u ", next_->ope2);

		}

		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_DELETE_REGISTER__) {
			printf(GET_PRINT_STR_SVR(next_->ope1));
		}
		else if (next_->sce_bmr_code == E_SCE_BINARY_INST_NAME_REPEAT__) {
			if (next_->ope1)	printf("%lu ", next_->ope1);
			else	printf("NO LIMIT ");
		}
		if (break_point) {
			char debug_str[128];
			putchar(';');
			(void)scanf("%128s", debug_str);
		}
		putchar('\n');
	}
}


#endif // !_SCE_IR_DEBUG_H_
