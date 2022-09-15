
#include "builder.h"
#include "func.h"
#include "context.h"
#include <stdlib.h>

void fkr_putBuilderAtEnd(fkr_builder* builder, fkr_blockRef block) {
    builder->block = block;
    builder->curr = NULL;
    fkr_val* curr = block->values;
    while(curr != NULL) {
        builder->curr = curr;
        curr = curr->next;
    }
    builder->ctx = block->fn->ctx;
}

static fkr_val* appendVal(fkr_builder* b, size_t size, fkr_valType type) {
    fkr_val* val = malloc(size);
    fkr_initVal(val);
    val->type = type;
    val->block = b->block;
    if(b->curr == NULL) {
        b->block->values = val;
    } else {
        b->curr->next = val;
    }
    b->curr = val;
    b->block->fn->hasSymbols = false;
    return val;
} 

fkr_valRef fkr_constI8(fkr_builder* b, int8_t val) {
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_i8Type();
    v->as.i8 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constI16(fkr_builder* b, int16_t val) { 
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_i16Type();
    v->as.i16 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constI32(fkr_builder* b, int32_t val) {
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_i32Type();
    v->as.i32 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constI64(fkr_builder* b, int64_t val) { 
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_i64Type();
    v->as.i64 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constU8(fkr_builder* b, uint8_t val) {
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_u8Type();
    v->as.u8 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constU16(fkr_builder* b, uint16_t val) { 
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_u16Type();
    v->as.u16 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constU32(fkr_builder* b, uint32_t val) {
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_u32Type();
    v->as.u32 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constU64(fkr_builder* b, uint64_t val) { 
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_u64Type();
    v->as.u64 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constF32(fkr_builder* b, float val) {
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_f32Type();
    v->as.f32 = val;
    return (fkr_val*)v;
}

fkr_valRef fkr_constF64(fkr_builder* b, double val) {
    fkr_valConst* v = (fkr_valConst*)appendVal(b, sizeof(fkr_valConst), FKR_VAL_CONST);
    v->v.valType = fkr_f64Type();
    v->as.f64 = val;
    return (fkr_val*)v;
}



fkr_valRef fkr_add(fkr_builder* builder, fkr_valRef a, fkr_valRef b) {
    fkr_valBinOp* v = (fkr_valBinOp*)appendVal(builder, sizeof(fkr_valBinOp), FKR_VAL_BINOP);
    v->a = a;
    v->b = b;
    v->op = FKR_OP_ADD;
    if(a->valType == b->valType && fkr_isNumericType(a->valType))
        v->v.valType = a->valType;
    else if(fkr_isPtrType(a->valType) && fkr_isIntegralType(b->valType))
        v->v.valType = ((fkr_typePtr*)a->valType)->elemType;
    else if(fkr_isPtrType(b->valType) && fkr_isIntegralType(a->valType))
        v->v.valType = ((fkr_typePtr*)b->valType)->elemType;
    else
        v->v.valType = NULL;
    return (fkr_valRef)v; 
}

fkr_valRef fkr_sub(fkr_builder* builder, fkr_valRef a, fkr_valRef b) {
    fkr_valBinOp* v = (fkr_valBinOp*)appendVal(builder, sizeof(fkr_valBinOp), FKR_VAL_BINOP);
    v->a = a;
    v->b = b;
    v->op = FKR_OP_SUB;
    if(a->valType == b->valType && fkr_isNumericType(a->valType))
        v->v.valType = a->valType;
    else if(fkr_isPtrType(a->valType) && fkr_isIntegralType(b->valType))
        v->v.valType = ((fkr_typePtr*)a->valType)->elemType;
    else if(fkr_isPtrType(b->valType) && fkr_isIntegralType(a->valType))
        v->v.valType = ((fkr_typePtr*)b->valType)->elemType;
    else
        v->v.valType = NULL;
    return (fkr_valRef)v; 
}

fkr_valRef fkr_mul(fkr_builder* builder, fkr_valRef a, fkr_valRef b) {
    fkr_valBinOp* v = (fkr_valBinOp*)appendVal(builder, sizeof(fkr_valBinOp), FKR_VAL_BINOP);
    v->a = a;
    v->b = b;
    v->op = FKR_OP_MUL;
    if(a->valType == b->valType && fkr_isNumericType(a->valType))
        v->v.valType = a->valType;
    else
        v->v.valType = NULL;
    return (fkr_valRef)v; 
}

fkr_valRef fkr_div(fkr_builder* builder, fkr_valRef a, fkr_valRef b) {
    fkr_valBinOp* v = (fkr_valBinOp*)appendVal(builder, sizeof(fkr_valBinOp), FKR_VAL_BINOP);
    v->a = a;
    v->b = b;
    v->op = FKR_OP_DIV;
    if(a->valType == b->valType && fkr_isNumericType(a->valType))
        v->v.valType = a->valType;
    else
        v->v.valType = NULL;
    return (fkr_valRef)v; 
}

fkr_valRef fkr_eq(fkr_builder* builder, fkr_valRef a, fkr_valRef b) {
    fkr_valBinOp* v = (fkr_valBinOp*)appendVal(builder, sizeof(fkr_valBinOp), FKR_VAL_BINOP);
    v->a = a;
    v->b = b;
    v->op = FKR_OP_EQ;
    if(a->valType == b->valType && fkr_isNumericType(a->valType))
        v->v.valType = fkr_u8Type();
    else
        v->v.valType = NULL;
    return (fkr_valRef)v; 
}



fkr_valRef fkr_alloc(fkr_builder* builder, fkr_typeRef type) {
    return fkr_arrAlloc(builder, type, fkr_constI64(builder, 1));
}

fkr_valRef fkr_arrAlloc(fkr_builder* builder, fkr_typeRef type, fkr_valRef cnt) {
    fkr_valAlloc* v = (fkr_valAlloc*)appendVal(builder, sizeof(fkr_valAlloc), FKR_VAL_ALLOC);
    v->cnt = cnt;
    v->v.valType = fkr_ptrType(builder->ctx, type);
    return (fkr_val*)v;
}



fkr_valRef fkr_get(fkr_builder* b, fkr_valRef ptr) {
    fkr_valGet* get = (fkr_valGet*)appendVal(b, sizeof(fkr_valGet), FKR_VAL_GET);
    get->ptr = ptr;
    if(fkr_isPtrType(ptr->valType)) {
        fkr_typePtr* ptrType = (fkr_typePtr*)ptr->valType;
        get->v.valType = ptrType->elemType;
    } else {
        get->v.valType = NULL;
    }
    return (fkr_val*)get;
}

void fkr_set(fkr_builder* b, fkr_valRef ptr, fkr_valRef val) {
    fkr_valSet* set = (fkr_valSet*)appendVal(b, sizeof(fkr_valSet), FKR_VAL_SET);
    set->v.valType = fkr_voidType();
    set->ptr = ptr;
    set->val = val;
}



void fkr_jump(fkr_builder* b, fkr_blockRef block) {
    fkr_valJump* jump = (fkr_valJump*)appendVal(b, sizeof(fkr_valJump), FKR_VAL_JUMP);
    jump->block = block;
    jump->v.valType = fkr_voidType();
}

void fkr_branch(fkr_builder* b, fkr_valRef cond, fkr_blockRef thenBlock, fkr_blockRef elseBlock) {
    fkr_valBranch* branch = (fkr_valBranch*)appendVal(b, sizeof(fkr_valBranch), FKR_VAL_BRANCH);
    branch->cond = cond;
    branch->thenBranch = thenBlock;
    branch->elseBranch = elseBlock;
    branch->v.valType = fkr_voidType();
}



fkr_valRef fkr_arg(fkr_builder* b, int idx) {
    fkr_valArg* arg = (fkr_valArg*)appendVal(b, sizeof(fkr_valArg), FKR_VAL_ARG);
    arg->idx = idx;
    fkr_typeFunc* fnType = (fkr_typeFunc*)b->block->fn->v.valType;
    if(idx >= 0 && idx < fnType->paramCnt)
        arg->v.valType = fnType->params[idx];
    else
        arg->v.valType = NULL;
    return (fkr_val*)arg; 
}

void fkr_return(fkr_builder* b, fkr_valRef val) {
    fkr_valReturn* ret = (fkr_valReturn*)appendVal(b, sizeof(fkr_valReturn), FKR_VAL_RETURN);
    ret->v.valType = fkr_voidType();
    ret->retVal = val;
    if(val == NULL) {
        ret->retType = fkr_voidType();
    } else {
        ret->retType = val->valType;
    }
}

fkr_valRef fkr_call(fkr_builder* b, fkr_valRef func, int argc, fkr_valRef* args) {
    fkr_valCall* call = (fkr_valCall*)appendVal(b, sizeof(fkr_valCall), FKR_VAL_CALL);
    call->func = func;
    call->argc = argc;
    call->args = malloc(argc * sizeof(fkr_val*));
    for(int i = 0; i < argc; i++)
        call->args[i] = args[i];

    if(fkr_getTypeType(func->valType) == FKR_TYPE_FUNC) {
        fkr_typeFunc* fType = (fkr_typeFunc*)func->valType;
        call->v.valType = fType->retType;
    } else {
        call->v.valType = NULL;
    }

    return (fkr_val*)call;
}

fkr_valRef fkr_struct(fkr_builder* b, int memCnt, fkr_valRef* members) {
    fkr_valStruct* strukt = (fkr_valStruct*)appendVal(b, sizeof(fkr_valStruct), FKR_VAL_STRUCT);
    strukt->memCnt = memCnt;
    strukt->members = malloc(memCnt * sizeof(fkr_val*));
    fkr_typeRef memTypes[memCnt];
    for(int i = 0; i < memCnt; i++) {
        strukt->members[i] = members[i];
        memTypes[i] = members[i]->valType;
    }
    strukt->v.valType = fkr_structType(b->ctx, memCnt, memTypes);
    return (fkr_val*)strukt;
}
