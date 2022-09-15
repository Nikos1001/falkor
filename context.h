
#ifndef FKR_CONTEXT_H
#define FKR_CONTEXT_H

#include "pkg.h"

typedef struct fkr_context {

    fkr_pkg* pkgs;

    fkr_type* types;

} fkr_context;

typedef fkr_context* fkr_contextRef;

fkr_contextRef fkr_makeContext();

void fkr_initContext(fkr_context* ctx);
void fkr_freeContext(fkr_contextRef ctx);

fkr_pkgRef fkr_addPackage(fkr_context* ctx, const char* name);

fkr_typeRef fkr_ptrType(fkr_contextRef ctx, fkr_typeRef elemType);
fkr_typeRef fkr_funcType(fkr_contextRef ctx, fkr_typeRef retType, int paramCnt, fkr_typeRef* params);
fkr_typeRef fkr_structType(fkr_contextRef ctx, int memCnt, fkr_typeRef* members);

#endif
