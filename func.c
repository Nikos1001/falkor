
#include "func.h"
#include "context.h"
#include <stdlib.h>
#include <string.h>

void fkr_initFunc(fkr_func* fn, fkr_str name, fkr_type* retType, int paramCnt, fkr_type** paramTypes, fkr_context* ctx) {
    fn->v.type = FKR_VAL_FUNC;

    fn->name = name;

    fn->blocks = NULL;
    fn->firstBlock = fkr_addBlock(fn, "entry");

    fn->ctx = ctx;

    fn->hasSymbols = false;
}

void fkr_freeFunc(fkr_func* fn) {
    fkr_freeStr(fn->name);
    fkr_block* curr = fn->blocks;
    while(curr != NULL) {
        fkr_block* next = curr->nextBlock;
        fkr_freeBlock(curr);
        free(curr);
        curr = next;
    }
}

fkr_blockRef fkr_addBlock(fkr_funcRef fn, const char* name) {
    fkr_block* block = malloc(sizeof(fkr_block));
    fkr_initBlock(block, name);
    block->nextBlock = fn->blocks;
    block->fn = fn;
    fn->blocks = block;
    fn->hasSymbols = false;
    return block;
}

fkr_builder fkr_makeFuncBuilder(fkr_funcRef fn) {
    fkr_builder res;
    fkr_putBuilderAtEnd(&res, fn->firstBlock);
    return res;
}

void fkr_generateFuncSymbols(fkr_func* fn) {

    if(fn->hasSymbols)
        return;

    uint64_t blkSym = 0;
    uint64_t valSym = 0;
    for(fkr_block* blk = fn->blocks; blk != NULL; blk = blk->nextBlock) {
        blk->sym = blkSym;
        blkSym++;
        for(fkr_val* val = blk->values; val != NULL; val = val->next) {
            val->sym = valSym;
            valSym++;
        }
    }
    fn->hasSymbols = true;
}

static void topologicalSortDFS(fkr_block* blk, bool* vis, fkr_block** stack, int* stackIdx) {

    if(vis[blk->sym])
        return;
    
    vis[blk->sym] = true;

    for(fkr_val* val = blk->values; val != NULL; val = val->next) {
        
        int nRefs = fkr_getNumValRefs(val);
        fkr_val* refs[nRefs];
        fkr_getValRefs(val, refs); 
        for(int i = 0; i < nRefs; i++)
            if(refs[i] != NULL && refs[i]->type != FKR_VAL_FUNC)
                topologicalSortDFS(refs[i]->block, vis, stack, stackIdx);

    }

    stack[*stackIdx] = blk;
    (*stackIdx)++; 
}

void fkr_blockTopologicalSort(fkr_func* fn) {
    fkr_generateFuncSymbols(fn);

    int blocks = 0;
    for(fkr_block* blk = fn->blocks; blk != NULL; blk = blk->nextBlock)
        blocks++;

    bool vis[blocks];
    for(int i = 0; i < blocks; i++)
        vis[i] = false;

    fkr_block* resStack[blocks];
    int idx = 0;

    for(fkr_block* blk = fn->blocks; blk != NULL; blk = blk->nextBlock) {
        topologicalSortDFS(blk, vis, resStack, &idx);
    }

    for(int i = 0; i < blocks - 1; i++) {
        resStack[i]->nextBlock = resStack[i + 1];
    }
    resStack[blocks - 1]->nextBlock = NULL;
    fn->firstBlock = resStack[0];
    fn->blocks = resStack[0];

    // Forces symbol reassignment, which is used to find value reference cycles later on
    fn->hasSymbols = false;

}