
#include "write.h"
#include <stdbool.h>
#include <stdlib.h>

static void writeValUse(fkr_str* s, fkr_val* v) {
    if(v->type == FKR_VAL_FUNC) {
        fkr_func* f = (fkr_func*)v;
        fkr_writeToStr(s, "%s", f->name.str);
    } else {
        fkr_writeToStr(s, "%%%d", v->sym);
    }
}

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
    } else if(t == FKR_TYPE_FUNC) {
        fkr_typeFunc* func = (fkr_typeFunc*)type;
        fkr_writeToStr(s, "(");
        for(int i = 0; i < func->paramCnt; i++) {
            fkr_writeType(s, func->params[i]);
            if(i != func->paramCnt - 1)
                fkr_writeToStr(s, ", ");
        }
        fkr_writeToStr(s, ") => ");
        fkr_writeType(s, func->retType);
    } else if(t == FKR_TYPE_STRUCT) {
        fkr_typeStruct* strukt = (fkr_typeStruct*)type;
        fkr_writeToStr(s, "{");
        for(int i = 0; i < strukt->memCnt; i++) {
            fkr_writeType(s, strukt->members[i]);
            if(i != strukt->memCnt - 1)
                fkr_writeToStr(s, ", ");
        }
        fkr_writeToStr(s, "}");
    }

}

void fkr_writeFuncDecl(fkr_str* s, fkr_func* fn) {

    fkr_typeFunc* fnType = (fkr_typeFunc*)fn->v.valType;

    fkr_writeType(s, fnType->retType); 
    fkr_writeToStr(s, " %s(", fn->name.str);
    
    bool placeComma = false;
    for(int i = 0; i < fnType->paramCnt; i++) {
        if(placeComma)
            fkr_writeToStr(s, ", ");
        fkr_writeType(s, fnType->params[i]);
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
        fkr_writeToStr(s, " ");
        writeValUse(s, val);
        fkr_writeToStr(s, " = ");
    }

    switch(val->type) {
        case FKR_VAL_CONST: {
            fkr_valConst* cst = (fkr_valConst*)val;
            fkr_writeConstVal(s, cst);
            break;
        }
        case FKR_VAL_BINOP: {
            fkr_valBinOp* binop = (fkr_valBinOp*)val;
            writeValUse(s, binop->a);
            fkr_writeToStr(s, " ");
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
            fkr_writeToStr(s, " ");
            writeValUse(s, binop->b);
            break;
        }
        case FKR_VAL_ALLOC: {
            fkr_valAlloc* alloc = (fkr_valAlloc*)val;
            fkr_writeToStr(s, "alloc ");
            fkr_writeType(s, ((fkr_typePtr*)val->valType)->elemType);
            fkr_writeToStr(s, "[", alloc->cnt->sym);
            writeValUse(s, alloc->cnt);
            fkr_writeToStr(s, "]", alloc->cnt->sym);
            break;
        }
        case FKR_VAL_GET: {
            fkr_valGet* get = (fkr_valGet*)val;
            fkr_writeToStr(s, "get ", get->ptr->sym);
            writeValUse(s, get->ptr);
            break;
        }
        case FKR_VAL_SET: {
            fkr_valSet* set = (fkr_valSet*)val;
            fkr_writeToStr(s, "set ");
            writeValUse(s, set->ptr);
            fkr_writeToStr(s, "<- ");
            writeValUse(s, set->val);
            break;
        }
        case FKR_VAL_JUMP: {
            fkr_valJump* jump = (fkr_valJump*)val;
            fkr_writeToStr(s, "jump %s", jump->block->name.str);
            break;
        }
        case FKR_VAL_BRANCH: {
            fkr_valBranch* branch = (fkr_valBranch*)val;
            fkr_writeToStr(s, "branch ");
            writeValUse(s, branch->cond);
            fkr_writeToStr(s, " then %s else %s", branch->thenBranch->name.str, branch->elseBranch->name.str);
            break;
        }
        case FKR_VAL_FUNC: {
            fkr_func* func = (fkr_func*)val;
            fkr_writeToStr(s, "%s", func->name.str);
            break;
        }
        case FKR_VAL_ARG: {
            fkr_valArg* arg = (fkr_valArg*)val;
            fkr_writeToStr(s, "@%d", arg->idx);
            break;
        }
        case FKR_VAL_RETURN: {
            fkr_valReturn* ret = (fkr_valReturn*)val;
            fkr_writeToStr(s, "return ");
            if(ret->retVal != NULL) {
                writeValUse(s, ret->retVal);
            }
            break;
        }
        case FKR_VAL_CALL: {
            fkr_valCall* call = (fkr_valCall*)val;
            writeValUse(s, call->func);
            fkr_writeToStr(s, "(");
            for(int i = 0; i < call->argc; i++) {
                writeValUse(s, call->args[i]);
                if(i != call->argc - 1)
                    fkr_writeToStr(s, ", ");
            }
            fkr_writeToStr(s, ")");
            break;
        }
    }

}
