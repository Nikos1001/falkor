
#include "block.h"
#include <stdlib.h>

void fkr_initBlock(fkr_block* block, const char* name) {
    block->values = NULL;
    block->name = fkr_makeStr(name);
}

void fkr_freeBlock(fkr_block* block) {
    fkr_val* curr = block->values;
    while(curr != NULL) {
        fkr_val* next = curr->next;
        fkr_freeVal(curr);
        free(curr);
        curr = next;
    }
    fkr_freeStr(block->name);
}
