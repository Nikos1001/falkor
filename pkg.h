
#ifndef FKR_PKG_H
#define FKR_PKG_H

#include "str.h"
#include "func.h"
#include "type.h"

typedef struct fkr_pkg {
    fkr_str name;
    struct fkr_func* funcs;

    struct fkr_context* ctx;

    struct fkr_pkg* nextPkg;
} fkr_pkg;

void fkr_initPkg(fkr_pkg* pkg, fkr_str name, struct fkr_context* ctx);
void fkr_freePkg(fkr_pkg* pkg);

typedef fkr_pkg* fkr_pkgRef;

fkr_funcRef fkr_addFunction(fkr_pkgRef pkg, const char* name, fkr_typeRef retType, int paramCnt, fkr_typeRef* paramTypes);

#endif
