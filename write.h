
#ifndef FKR_WRITE_H
#define FKR_WRITE_H

#include "str.h"
#include "pkg.h"

void fkr_writeConstVal(fkr_str* s, fkr_valConst* cst);
void fkr_writeType(fkr_str* s, fkr_type* type);
void fkr_writeFuncDecl(fkr_str* s, fkr_func* fn);
void fkr_writeBlock(fkr_str* s, fkr_block* block, fkr_val* highlight);
void fkr_writeVal(fkr_str* s, fkr_val* val);

#endif
