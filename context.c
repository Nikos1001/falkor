
#include "context.h"

#include <stdlib.h>

fkr_contextRef fkr_makeContext() {
    fkr_context* ctx = malloc(sizeof(ctx));
    fkr_initContext(ctx);
    return ctx;
}

void fkr_initContext(fkr_context* ctx) {
    ctx->pkgs = NULL;
    ctx->types = NULL;
}

void fkr_freeContext(fkr_contextRef ctx) {
    fkr_pkg* curr = ctx->pkgs;
    while(curr != NULL) {
        fkr_pkg* next = curr->nextPkg;
        fkr_freePkg(curr);
        free(curr);
        curr = next;
    }
    
    fkr_type* currType = ctx->types;
    while(currType != NULL) {
        fkr_type* nextType = currType->nextType;
        fkr_freeType(currType);
        free(currType);
        currType = nextType;
    }

    free(ctx);
}

fkr_pkgRef fkr_addPackage(fkr_context* ctx, const char* name) {
    fkr_pkg* pkg = malloc(sizeof(fkr_pkg));
    fkr_initPkg(pkg, fkr_makeStr(name), ctx);
    pkg->nextPkg = ctx->pkgs;
    ctx->pkgs = pkg;
    return pkg;
}


static fkr_type* allocType(fkr_context* ctx, fkr_typeType type, size_t size) {
    fkr_type* t = malloc(size);
    t->type = type;
    t->nextType = ctx->types;
    ctx->types = t;
    return t;
}

fkr_typeRef fkr_ptrType(fkr_contextRef ctx, fkr_typeRef elemType) {
    for(fkr_type* t = ctx->types; t != NULL; t = t->nextType) {
        if(fkr_getTypeType(t) == FKR_TYPE_PTR) {
            fkr_typePtr* ptr = (fkr_typePtr*)t;
            if(ptr->elemType == elemType)
                return t;
        }
    }
    fkr_typePtr* res = (fkr_typePtr*)allocType(ctx, FKR_TYPE_PTR, sizeof(fkr_typePtr));
    res->elemType = elemType;
    return (fkr_type*)res;
}

fkr_typeRef fkr_funcType(fkr_contextRef ctx, fkr_typeRef retType, int paramCnt, fkr_typeRef* params) {
    for(fkr_type* t = ctx->types; t != NULL; t = t->nextType) {
        if(fkr_getTypeType(t) == FKR_TYPE_FUNC) {
            fkr_typeFunc* func = (fkr_typeFunc*)t;

            if(func->retType != retType)
                continue;
            if(func->paramCnt != paramCnt)
                continue;

            bool paramsSame = true;
            for(int i = 0; i < paramCnt; i++) {
                if(func->params[i] != params[i])
                    paramsSame = false;
            } 
            if(!paramsSame)
                continue;

            return t;
        }
    }
    fkr_typeFunc* res = (fkr_typeFunc*)allocType(ctx, FKR_TYPE_FUNC, sizeof(fkr_typeFunc));
    res->retType = retType;
    res->paramCnt = paramCnt;
    res->params = malloc(paramCnt * sizeof(fkr_type*));
    for(int i = 0; i < paramCnt; i++)
        res->params[i] = params[i];
    return (fkr_type*)res;
}

fkr_typeRef fkr_structType(fkr_contextRef ctx, int memCnt, fkr_typeRef* members) {
    for(fkr_type* t = ctx->types; t != NULL; t = t->nextType) {
        if(fkr_getTypeType(t) == FKR_TYPE_STRUCT) {
            fkr_typeStruct* strukt = (fkr_typeStruct*)t;
            if(strukt->memCnt != memCnt)
                continue;

            bool match = true;
            for(int i = 0; i < memCnt; i++) {
                if(strukt->members[i] != members[i]) {
                    match = false;
                    break;
                }
            }
            if(!match)
                continue;
            
            return t;
        }
    }

    fkr_typeStruct* strukt = (fkr_typeStruct*)allocType(ctx, FKR_TYPE_STRUCT, sizeof(fkr_typeStruct));
    strukt->memCnt = memCnt;
    strukt->members = malloc(memCnt * sizeof(fkr_type*));
    for(int i = 0; i < memCnt; i++)
        strukt->members[i] = members[i];

    return (fkr_type*)strukt; 
}
