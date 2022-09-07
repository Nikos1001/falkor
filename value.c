
#include "value.h"
#include <stdlib.h>

void fkr_initVal(fkr_val* val) {

}

void fkr_freeVal(fkr_val* val) {

}

size_t fkr_valPtrOffsets[][FKR_MAX_VAL_PTRS] = {
    [FKR_VAL_CONST]  = {0,                               0,                         0}, 
    [FKR_VAL_BINOP]  = {offsetof(fkr_valBinOp, a),       offsetof(fkr_valBinOp, b), 0},
    [FKR_VAL_ALLOC]  = {offsetof(fkr_valAlloc, cnt),     0,                         0},
    [FKR_VAL_GET]    = {offsetof(fkr_valGet, ptr),       0,                         0},
    [FKR_VAL_SET]    = {offsetof(fkr_valSet, ptr),       offsetof(fkr_valSet, val), 0},
    [FKR_VAL_JUMP]   = {0,                               0,                         0},
    [FKR_VAL_BRANCH] = {offsetof(fkr_valBranch, cond),   0,                         0},
    [FKR_VAL_RETURN] = {offsetof(fkr_valReturn, retVal), 0,                         0}
};
