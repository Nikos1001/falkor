
#ifndef FKR_FUNC_H
#define FKR_FUNC_H

#include "str.h"
#include "block.h"
#include "builder.h"
#include <stdbool.h>

typedef struct fkr_func {
    fkr_val v;

    fkr_str name;
    fkr_block* blocks;
    fkr_block* firstBlock;

    struct fkr_context* ctx;

    bool hasSymbols;

    struct fkr_func* nextFunc;
} fkr_func;

typedef fkr_func* fkr_funcRef;

void fkr_initFunc(fkr_func* fn, fkr_str name, fkr_type* retType, int paramCnt, fkr_type** paramTypes, struct fkr_context* ctx);
void fkr_freeFunc(fkr_func* fn);

fkr_blockRef fkr_addBlock(fkr_funcRef fn, const char* name);

fkr_builder fkr_makeFuncBuilder(fkr_funcRef fn);

void fkr_generateFuncSymbols(fkr_func* fn);

void fkr_blockTopologicalSort(fkr_func* fn);

#endif
