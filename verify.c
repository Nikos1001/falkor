
#include "verify.h"
#include "write.h"
#include <stdlib.h>
#include <stdbool.h>

static void verifyVal(fkr_error* err, fkr_func* func, fkr_block* block, fkr_val* val) {

    bool causedError = false;

    switch(val->type) {
        case FKR_VAL_CONST:
            break;
        case FKR_VAL_BINOP: {
            fkr_valBinOp* binop = (fkr_valBinOp*)val;
            if(fkr_getTypeType(val->valType) == FKR_TYPE_ERR) {
                causedError = true;

                fkr_str aType = fkr_makeStr("");
                fkr_str bType = fkr_makeStr("");
                fkr_writeType(&aType, binop->a->valType);
                fkr_writeType(&bType, binop->b->valType);

                switch(binop->op) {
                    case FKR_OP_ADD: {
                        fkr_writeToStr(&err->msg, "Cannot add %s and %s!\n", aType.str, bType.str);
                        break;
                    }
                    case FKR_OP_SUB: {
                        fkr_writeToStr(&err->msg, "Cannot subtract %s from %s!\n", bType.str, aType.str);
                        break;
                    }
                    case FKR_OP_MUL: {
                        fkr_writeToStr(&err->msg, "Cannot multiply %s by %s!\n", aType.str, bType.str);
                        break;
                    }
                    case FKR_OP_DIV: {
                        fkr_writeToStr(&err->msg, "Cannot divide %s by %s!\n", aType.str, bType.str);
                        break;
                    }
                    case FKR_OP_EQ: {
                        fkr_writeToStr(&err->msg, "Cannot compare %s and %s!\n", aType.str, bType.str);
                        break;
                    }
                }

                fkr_freeStr(aType);
                fkr_freeStr(bType);

            }
            break;
        }
        case FKR_VAL_ALLOC: {
            fkr_valAlloc* alloc = (fkr_valAlloc*)val;
            if(!fkr_isIntegralType(alloc->cnt->valType)) {
                causedError = true;
                fkr_writeToStr(&err->msg, "Alloc element count must be an integer!\n");
            }
            break;
        }
        case FKR_VAL_GET: {
            fkr_valGet* get = (fkr_valGet*)val;
            if(!fkr_isPtrType(get->ptr->valType)) {
                causedError = true;
                fkr_writeToStr(&err->msg, "Cannot dereference ");
                fkr_writeType(&err->msg, get->ptr->valType);
                fkr_writeToStr(&err->msg, "!\n");
            }
            break;
        }
        case FKR_VAL_SET: {
            fkr_valSet* set = (fkr_valSet*)val;
            if(!fkr_isPtrType(set->ptr->valType)) {
                causedError = true;
                fkr_writeToStr(&err->msg, "Cannot set a non-pointer!\n");
                fkr_writeToStr(&err->msg, "Pointer has type ");
                fkr_writeType(&err->msg, set->ptr->valType);
                fkr_writeToStr(&err->msg, ".\n");
                break;
            }
            fkr_typePtr* ptrType = (fkr_typePtr*)set->ptr->valType;
            if(ptrType->elemType != set->val->valType) {
                causedError = true;
                fkr_writeToStr(&err->msg, "Pointer type does not match value type!\n");
                fkr_writeToStr(&err->msg, "Tried to set ");
                fkr_writeType(&err->msg, (fkr_type*)ptrType);
                fkr_writeToStr(&err->msg, " to ");
                fkr_writeType(&err->msg, set->val->valType);
                fkr_writeToStr(&err->msg, ".\n");
            }
            break;
        }
        case FKR_VAL_JUMP: {
            break;
        }
        case FKR_VAL_BRANCH: {
            fkr_valBranch* branch = (fkr_valBranch*)val;
            if(!fkr_isIntegralType(branch->cond->valType)) {
                causedError = true;
                fkr_writeToStr(&err->msg, "Branch condition must be an integer, not ");
                fkr_writeType(&err->msg, branch->cond->valType);
                fkr_writeToStr(&err->msg, "!\n");
            }
            break;
        }
        case FKR_VAL_RETURN: {
            fkr_valReturn* ret = (fkr_valReturn*)val;
            if(ret->retType != func->retType) {
                causedError = true;
                fkr_writeToStr(&err->msg, "Invalid return type!\n");
                fkr_writeToStr(&err->msg, "Function %s returns ", func->name.str);
                fkr_writeType(&err->msg, func->retType);
                fkr_writeToStr(&err->msg, ", received ");
                fkr_writeType(&err->msg, ret->retType);
                fkr_writeToStr(&err->msg, ".\n");
            }
        }
    }
    
    if(causedError) {
        err->error = true;
        fkr_writeFuncDecl(&err->msg, func);
        fkr_writeToStr(&err->msg, "\n");
        if(block != func->firstBlock) {
            fkr_writeToStr(&err->msg, "\t...\n");
        }
        fkr_writeBlock(&err->msg, block, val);
    }
}

static void verifyFunc(fkr_error* err, fkr_func* func) {

    fkr_generateFuncSymbols(func);

    for(fkr_block* block = func->blocks; block != NULL; block = block->nextBlock) {

        fkr_val* terminatorVal = NULL;
        for(fkr_val* val = block->values; val != NULL; val = val->next) {
            terminatorVal = val;
            verifyVal(err, func, block, val);
        }

        if(terminatorVal == NULL || (terminatorVal->type != FKR_VAL_RETURN && terminatorVal->type != FKR_VAL_JUMP && terminatorVal->type != FKR_VAL_BRANCH)) {
            err->error = true;
            fkr_writeToStr(&err->msg, "Block %s doesn't have terminator!\n", block->name.str);
            fkr_writeFuncDecl(&err->msg, func);
            fkr_writeToStr(&err->msg, "\n");
            if(block != func->firstBlock)
                fkr_writeToStr(&err->msg, "\t...\n");
            fkr_writeBlock(&err->msg, block, terminatorVal);
        }

    }

}

fkr_error fkr_verifyPkg(fkr_pkgRef pkg) {

    fkr_error err;
    err.msg.str = NULL;

    for(fkr_func* fn = pkg->funcs; fn != NULL; fn = fn->nextFunc) {
        verifyFunc(&err, fn);
    }

    return err;

}
