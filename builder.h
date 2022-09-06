
#ifndef FKR_BUILDER_H
#define FKR_BUILDER_H

#include "block.h"
#include <stdint.h>

struct fkr_context;

typedef struct {
    fkr_val* curr;
    fkr_block* block;
    struct fkr_context* ctx;
} fkr_builder;

void fkr_putBuilderAtEnd(fkr_builder* builder, fkr_blockRef block);

fkr_valRef fkr_constI8(fkr_builder* b, int8_t val);
fkr_valRef fkr_constI16(fkr_builder* b, int16_t val);
fkr_valRef fkr_constI32(fkr_builder* b, int32_t val);
fkr_valRef fkr_constI64(fkr_builder* b, int64_t val);
fkr_valRef fkr_constU8(fkr_builder* b, uint8_t val);
fkr_valRef fkr_constU16(fkr_builder* b, uint16_t val);
fkr_valRef fkr_constU32(fkr_builder* b, uint32_t val);
fkr_valRef fkr_constU64(fkr_builder* b, uint64_t val);
fkr_valRef fkr_constF32(fkr_builder* b, float val);
fkr_valRef fkr_constF64(fkr_builder* b, double val);

fkr_valRef fkr_add(fkr_builder* builder, fkr_valRef a, fkr_valRef b);
fkr_valRef fkr_sub(fkr_builder* builder, fkr_valRef a, fkr_valRef b);
fkr_valRef fkr_mul(fkr_builder* builder, fkr_valRef a, fkr_valRef b);
fkr_valRef fkr_div(fkr_builder* builder, fkr_valRef a, fkr_valRef b);
fkr_valRef fkr_eq(fkr_builder* builder, fkr_valRef a, fkr_valRef b);

fkr_valRef fkr_alloc(fkr_builder* builder, fkr_typeRef type);
fkr_valRef fkr_arrAlloc(fkr_builder* builder, fkr_typeRef type, fkr_valRef cnt);

fkr_valRef fkr_get(fkr_builder* b, fkr_valRef ptr);
void fkr_set(fkr_builder* b, fkr_valRef ptr, fkr_valRef val);

void fkr_jump(fkr_builder* b, fkr_blockRef block);
void fkr_branch(fkr_builder* b, fkr_valRef cond, fkr_blockRef thenBlock, fkr_blockRef elseBlock);

void fkr_return(fkr_builder* b, fkr_valRef val);

#endif
