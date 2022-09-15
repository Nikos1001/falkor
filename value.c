
#include "value.h"
#include <stdlib.h>

void fkr_initVal(fkr_val* val) {

}

void fkr_freeVal(fkr_val* val) {
    if(val->type == FKR_VAL_CALL) {
        fkr_valCall* call = (fkr_valCall*)val;
        free(call->args);
    } else if(val->type == FKR_VAL_STRUCT) {
        fkr_valStruct* strukt = (fkr_valStruct*)val;
        free(strukt->members);
    }
}

int fkr_getNumValRefs(fkr_val* v) {
    int res = 0;
    for(int i = 0; i < FKR_MAX_VAL_PTRS; i++)
        if(fkr_valPtrOffsets[v->type][i] != 0)
            res++;
    if(v->type == FKR_VAL_CALL) {
        fkr_valCall* call = (fkr_valCall*)v;
        res += call->argc; 
    } else if(v->type == FKR_VAL_STRUCT) {
        fkr_valStruct* strukt = (fkr_valStruct*)v;
        res += strukt->memCnt; 
    }
    return res;
}

void fkr_getValRefs(fkr_val* v, fkr_val** dst) {
    for(int i = 0; i < FKR_MAX_VAL_PTRS; i++) {
        if(fkr_valPtrOffsets[v->type][i] != 0) {
            *dst = *(fkr_val**)((char*)v + fkr_valPtrOffsets[v->type][i]);
            dst++;
        }
    }
    if(v->type == FKR_VAL_CALL) {
        fkr_valCall* call = (fkr_valCall*)v;
        for(int i = 0; i < call->argc; i++) {
            *dst = call->args[i];
            dst++;
        }
    } else if(v->type == FKR_VAL_STRUCT) {
        fkr_valStruct* strukt = (fkr_valStruct*)v;
        for(int i = 0; i < strukt->memCnt; i++) {
            *dst = strukt->members[i];
            dst++;
        }
    }
}

size_t fkr_valPtrOffsets[][FKR_MAX_VAL_PTRS] = {
    [FKR_VAL_CONST]  = {0,                               0,                         0}, 
    [FKR_VAL_BINOP]  = {offsetof(fkr_valBinOp, a),       offsetof(fkr_valBinOp, b), 0},
    [FKR_VAL_ALLOC]  = {offsetof(fkr_valAlloc, cnt),     0,                         0},
    [FKR_VAL_GET]    = {offsetof(fkr_valGet, ptr),       0,                         0},
    [FKR_VAL_SET]    = {offsetof(fkr_valSet, ptr),       offsetof(fkr_valSet, val), 0},
    [FKR_VAL_JUMP]   = {0,                               0,                         0},
    [FKR_VAL_BRANCH] = {offsetof(fkr_valBranch, cond),   0,                         0},
    [FKR_VAL_FUNC]   = {0,                               0,                         0},
    [FKR_VAL_RETURN] = {offsetof(fkr_valReturn, retVal), 0,                         0},
    [FKR_VAL_CALL]   = {offsetof(fkr_valCall, func),     0,                         0},
    [FKR_VAL_STRUCT] = {0,                               0,                         0}
};
