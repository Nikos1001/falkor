
#include "compiler.h"
#include "write.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void symToStr(char* str, uint64_t sym) {
    if(sym == 0) {
        str[0] = 'a';
        str[1] = '\0';
        return;
    }

    while(sym > 0) {
        *str = 'a' + (sym % 26);
        str++;
        sym /= 26;
    }
    *str = '\0';
}

static void compileType(fkr_str* src, fkr_type* t) {
    if(fkr_isPrimitiveType(t)) {
        fkr_writeToStr(src, "%s", fkr_typeNames[fkr_getTypeType(t)].cName);
    }
    fkr_typeType type = fkr_getTypeType(t);
    if(type == FKR_TYPE_PTR) {
        fkr_typePtr* ptr = (fkr_typePtr*)t;
        compileType(src, ptr->elemType);
        fkr_writeToStr(src, "*");
    }
}

static void compileValUse(fkr_str* src, fkr_val* v) {
    char valName[32];
    symToStr(valName, v->sym);
    fkr_writeToStr(src, "%s", valName);
}

static void compileVal(fkr_str* src, fkr_val* val) {

    char valName[32];
    symToStr(valName, val->sym);

    if(val->type == FKR_VAL_ALLOC) {
        fkr_valAlloc* alloc = (fkr_valAlloc*)val;
        compileType(src, ((fkr_typePtr*)val->valType)->elemType);
        fkr_writeToStr(src, " alloc_%s[", valName);
        compileValUse(src, alloc->cnt);
        fkr_writeToStr(src, "];\n");
    }

    if(val->valType != fkr_voidType()) {
        compileType(src, val->valType);
        fkr_writeToStr(src, " %s = ", valName);
    }
    
    switch(val->type) {
        case FKR_VAL_CONST: {
            fkr_valConst* cst = (fkr_valConst*)val;
            fkr_writeConstVal(src, cst);
            break;
        }
        case FKR_VAL_BINOP: {
            fkr_valBinOp* binop = (fkr_valBinOp*)val;
            compileValUse(src, binop->a);
            switch(binop->op) {
                case FKR_OP_ADD: {
                    fkr_writeToStr(src, " + ");
                    break;
                }
                case FKR_OP_SUB: {
                    fkr_writeToStr(src, " - ");
                    break;
                }
                case FKR_OP_MUL: {
                    fkr_writeToStr(src, " * ");
                    break;
                }
                case FKR_OP_DIV: {
                    fkr_writeToStr(src, " / ");
                    break;
                }
                case FKR_OP_EQ: {
                    fkr_writeToStr(src, " == ");
                }
            }
            compileValUse(src, binop->b);
            break;
        }
        case FKR_VAL_ALLOC: {
            fkr_writeToStr(src, "alloc_%s", valName);
            break;
        }
        case FKR_VAL_GET: {
            fkr_valGet* get = (fkr_valGet*)val;
            fkr_writeToStr(src, "*");
            compileValUse(src, get->ptr);
            break;
        }
        case FKR_VAL_SET: {
            fkr_valSet* set = (fkr_valSet*)val;
            fkr_writeToStr(src, "*");
            compileValUse(src, set->ptr);
            fkr_writeToStr(src, " = ");
            compileValUse(src, set->val);
            break;
        }
        case FKR_VAL_JUMP: {
            fkr_valJump* jump = (fkr_valJump*)val;
            char buf[32];
            symToStr(buf, jump->block->sym);
            fkr_writeToStr(src, "goto %s_%s", jump->block->name.str, buf);
            break;
        }
        case FKR_VAL_BRANCH: {
            fkr_valBranch* branch = (fkr_valBranch*)val;
            char thenBuf[32];
            symToStr(thenBuf, branch->thenBranch->sym);
            char elseBuf[32];
            symToStr(elseBuf, branch->elseBranch->sym);
            fkr_writeToStr(src, "if(");
            compileValUse(src, branch->cond);
            fkr_writeToStr(src, ") { goto %s_%s; } else { goto %s_%s; }", branch->thenBranch->name.str, thenBuf, branch->elseBranch->name.str, elseBuf);
            break;
        }
        case FKR_VAL_RETURN: {
            fkr_valReturn* ret = (fkr_valReturn*)val;
            fkr_writeToStr(src, "return ");
            if(ret->retVal != NULL) {
                compileValUse(src, ret->retVal);
            }
            break;
        }
    }

    fkr_writeToStr(src, ";\n");
}

static void compileFunc(fkr_str* src, fkr_func* fn) {

    fkr_generateFuncSymbols(fn);

    fkr_blockTopologicalSort(fn);

    for(fkr_block* block = fn->blocks; block != NULL; block = block->nextBlock) {
        char blockName[32];
        symToStr(blockName, block->sym);
        fkr_writeToStr(src, "%s_%s: (void)0;\n", block->name.str, blockName);
        for(fkr_val* val = block->values; val != NULL; val = val->next) {
            compileVal(src, val);
        }
    }
}

void fkr_compilePkg(fkr_pkgRef pkg) {
    char filePath[pkg->name.len + 3];
    memcpy(filePath, pkg->name.str, pkg->name.len);
    memcpy(filePath + pkg->name.len, ".h", 3);
    FILE* headerFile = fopen(filePath, "w");
    memcpy(filePath + pkg->name.len, ".c", 3);
    FILE* sourceFile = fopen(filePath, "w");

    fprintf(headerFile, "#include <stdint.h>\n");
    fprintf(sourceFile, "#include \"%s.h\"\n", pkg->name.str);

    fkr_str source = fkr_makeStr("");
    fkr_str header = fkr_makeStr("");

    for(fkr_func* fn = pkg->funcs; fn != NULL; fn = fn->nextFunc) {
        compileType(&header, fn->retType);
        fkr_writeToStr(&header, " %s();\n", fn->name.str);

        compileType(&source, fn->retType);
        fkr_writeToStr(&source, " %s() {\n", fn->name.str);
        compileFunc(&source, fn);
        fkr_writeToStr(&source, "}\n");
    }

    fprintf(sourceFile, "%s", source.str);
    fprintf(headerFile, "%s", header.str);
    
    fkr_freeStr(source);
    fkr_freeStr(header);

    fclose(headerFile);
    fclose(sourceFile);
}

void fkr_compile(fkr_context* ctx) {
    for(fkr_pkg* pkg = ctx->pkgs; pkg != NULL; pkg = pkg->nextPkg)
        fkr_compilePkg(pkg);
}
