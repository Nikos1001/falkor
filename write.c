
#include "write.h"
#include <stdbool.h>
#include <stdlib.h>

void fkr_writeConstVal(fkr_str* s, fkr_valConst* cst) {
    switch(fkr_getTypeType(cst->v.valType)) {
        case FKR_TYPE_I8: {
            fkr_writeToStr(s, "%lli", cst->as.i8);
            break;
        }
        case FKR_TYPE_I16: {
            fkr_writeToStr(s, "%lli", cst->as.i16);
            break;
        }
        case FKR_TYPE_I32: {
            fkr_writeToStr(s, "%lli", cst->as.i32);
            break;
        }
        case FKR_TYPE_I64: {
            fkr_writeToStr(s, "%lli", cst->as.i64);
            break;
        }
        case FKR_TYPE_U8: {
            fkr_writeToStr(s, "%llu", cst->as.u8);
            break;
        }
        case FKR_TYPE_U16: {
            fkr_writeToStr(s, "%llu", cst->as.u16);
            break;
        }
        case FKR_TYPE_U32: {
            fkr_writeToStr(s, "%llu", cst->as.u32);
            break;
        }
        case FKR_TYPE_U64: {
            fkr_writeToStr(s, "%llu", cst->as.u64);
            break;
        }
        case FKR_TYPE_F32: {
            fkr_writeToStr(s, "%g", cst->as.f32);
            break;
        }
        case FKR_TYPE_F64: {
            fkr_writeToStr(s, "%g", cst->as.f64);
            break;
        }
        default:
            break;
    }
}

void fkr_writeType(fkr_str* s, fkr_type* type) {
    
    fkr_typeType t = fkr_getTypeType(type);
    if(fkr_isPrimitiveType(type)) {
        fkr_writeToStr(s, "%s", fkr_typeNames[t].name);
    }

    if(t == FKR_TYPE_PTR) {
        fkr_typePtr* ptr = (fkr_typePtr*)type;
        fkr_writeType(s, ptr->elemType);
        fkr_writeToStr(s, "*");
    }

}

void fkr_writeFuncDecl(fkr_str* s, fkr_func* fn) {

    fkr_writeType(s, fn->retType);
    fkr_writeToStr(s, " %s(", fn->name.str);
    
    bool placeComma = false;
    for(int i = 0; i < fn->paramCnt; i++) {
        if(placeComma)
            fkr_writeToStr(s, ", ");
        fkr_writeType(s, fn->paramTypes[i]);
        fkr_writeToStr(s, " @%d", i);
    }

    fkr_writeToStr(s, ")");

}

void fkr_writeBlock(fkr_str* s, fkr_block* block, fkr_val* highlight) {

    fkr_writeToStr(s, "%s:\n", block->name.str);
    for(fkr_val* val = block->values; val != NULL; val = val->next) {
        fkr_writeToStr(s, "\t");

        int initialErrMsgLen = s->len;

        fkr_writeVal(s, val);

        int valLen = s->len - initialErrMsgLen;

        fkr_writeToStr(s, "\n");

        if(highlight == val) {
            fkr_writeToStr(s, "\t");
            fkr_writeToStr(s, "^");
            for(int i = 0; i < valLen - 1; i++)
                fkr_writeToStr(s, "~");
            fkr_writeToStr(s, "\n");
        }
    }

}

void fkr_writeVal(fkr_str* s, fkr_val* val) {

    if(val->valType != fkr_voidType()) {
        fkr_writeType(s, val->valType);
        fkr_writeToStr(s, " %%%d = ", val->sym);
    }

    switch(val->type) {
        case FKR_VAL_CONST: {
            fkr_valConst* cst = (fkr_valConst*)val;
            fkr_writeConstVal(s, cst);
            break;
        }
        case FKR_VAL_BINOP: {
            fkr_valBinOp* binop = (fkr_valBinOp*)val;
            fkr_writeToStr(s, "%%%d ", binop->a->sym);
            switch(binop->op) {
                case FKR_OP_ADD: {
                    fkr_writeToStr(s, "+");
                    break;
                }
                case FKR_OP_SUB: {
                    fkr_writeToStr(s, "-");
                    break;
                }
                case FKR_OP_MUL: {
                    fkr_writeToStr(s, "*");
                    break;
                }
                case FKR_OP_DIV: {
                    fkr_writeToStr(s, "/");
                    break;
                }
                case FKR_OP_EQ: {
                    fkr_writeToStr(s, "==");
                }
            }
            fkr_writeToStr(s, " %%%d", binop->b->sym);
            break;
        }
        case FKR_VAL_ALLOC: {
            fkr_valAlloc* alloc = (fkr_valAlloc*)val;
            fkr_writeToStr(s, "alloc ");
            fkr_writeType(s, ((fkr_typePtr*)val->valType)->elemType);
            fkr_writeToStr(s, "[%%%d]", alloc->cnt->sym);
            break;
        }
        case FKR_VAL_GET: {
            fkr_valGet* get = (fkr_valGet*)val;
            fkr_writeToStr(s, "get %%%d", get->ptr->sym);
            break;
        }
        case FKR_VAL_SET: {
            fkr_valSet* set = (fkr_valSet*)val;
            fkr_writeToStr(s, "set %%%d <- %%%d", set->ptr->sym, set->val->sym);
            break;
        }
        case FKR_VAL_JUMP: {
            fkr_valJump* jump = (fkr_valJump*)val;
            fkr_writeToStr(s, "jump %s", jump->block->name.str);
            break;
        }
        case FKR_VAL_BRANCH: {
            fkr_valBranch* branch = (fkr_valBranch*)val;
            fkr_writeToStr(s, "branch %%%d then %s else %s", branch->cond->sym, branch->thenBranch->name.str, branch->elseBranch->name.str);
            break;
        }
        case FKR_VAL_RETURN: {
            fkr_valReturn* ret = (fkr_valReturn*)val;
            fkr_writeToStr(s, "return ");
            if(ret->retVal != NULL) {
                fkr_writeToStr(s, "%%%d", ret->retVal->sym);
            }
            break;
        }
    }

}
