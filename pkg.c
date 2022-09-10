
#include "pkg.h"
#include "context.h"
#include <stdlib.h>

void fkr_initPkg(fkr_pkg* pkg, fkr_str name, struct fkr_context* ctx) {
    pkg->name = name;
    pkg->funcs = NULL;
    pkg->ctx = ctx;
} 

void fkr_freePkg(fkr_pkg* pkg) {
    fkr_func* curr = pkg->funcs;
    while(curr != NULL) {
        fkr_func* next = curr->nextFunc;
        fkr_freeFunc(curr);
        free(curr);
        curr = next;
    }
}

fkr_funcRef fkr_addFunction(fkr_pkgRef pkg, const char* name, fkr_typeRef retType, int paramCnt, fkr_typeRef* paramTypes) {
    fkr_func* func = malloc(sizeof(fkr_func));
    fkr_initFunc(func, fkr_makeStr(name), retType, paramCnt, paramTypes, pkg->ctx);
    func->nextFunc = pkg->funcs;
    pkg->funcs = func;
    func->v.valType = fkr_funcType(pkg->ctx, retType, paramCnt, paramTypes);
    return func;
}
