
#ifndef FKR_BLOCK_H
#define FKR_BLOCK_H

#include "value.h"
#include "str.h"

struct fkr_func;

typedef struct fkr_block {

    fkr_val* values;
    fkr_str name;
    
    uint64_t sym;

    struct fkr_func* fn;

    struct fkr_block* nextBlock;

} fkr_block;

void fkr_initBlock(fkr_block* block, const char* name);
void fkr_freeBlock(fkr_block* block);

typedef fkr_block* fkr_blockRef;

#endif
