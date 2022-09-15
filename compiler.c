
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
    } else if(type == FKR_TYPE_FUNC) {
        fkr_writeToStr(src, "void*"); // curse you, C function pointer syntax 
    } else if(type == FKR_TYPE_STRUCT) {
        fkr_typeStruct* strukt = (fkr_typeStruct*)t;
        fkr_writeToStr(src, "struct {");
        for(int i = 0; i < strukt->memCnt; i++) {
            compileType(src, strukt->members[i]);
            fkr_writeToStr(src, " m%d;", i);
        } 
        fkr_writeToStr(src, "}");
    }
}

static void compileValUse(fkr_str* src, fkr_val* v) {
    if(v->type == FKR_VAL_FUNC) {
        fkr_func* f = (fkr_func*)v;
        fkr_writeToStr(src, "%s", f->name.str);
    } else {
        char valName[32];
        symToStr(valName, v->sym);
        fkr_writeToStr(src, "%s", valName);
    }
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
            fkr_writeToStr(src, "(void*)alloc_%s", valName);
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
            
            if(fkr_getTypeType(set->val->valType) == FKR_TYPE_STRUCT) {
                fkr_typeStruct* strukt = (fkr_typeStruct*)set->val->valType;
                for(int i = 0; i < strukt->memCnt; i++) {
                    compileValUse(src, set->ptr);
                    fkr_writeToStr(src, "->m%d = ", i);
                    compileValUse(src, set->val);
                    fkr_writeToStr(src, ".m%d;\n", i);
                }
            } else {
                fkr_writeToStr(src, "*");
                compileValUse(src, set->ptr);
                fkr_writeToStr(src, " = ");
                compileValUse(src, set->val);
            }
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
        case FKR_VAL_ARG: {
            fkr_valArg* arg = (fkr_valArg*)val;
            fkr_writeToStr(src, "a_%d", arg->idx);
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
        case FKR_VAL_CALL: {
            fkr_valCall* call = (fkr_valCall*)val;

            fkr_typeFunc* fnType = (fkr_typeFunc*)call->func->valType;
            fkr_writeToStr(src, "((");
            compileType(src, fnType->retType);
            fkr_writeToStr(src, " (*)(");
            for(int i = 0; i < fnType->paramCnt; i++) {
                compileType(src, fnType->params[i]);
                if(i != fnType->paramCnt - 1)
                    fkr_writeToStr(src, ", ");
            }
            fkr_writeToStr(src, "))"); 

            compileValUse(src, call->func);
            fkr_writeToStr(src, ")");

            fkr_writeToStr(src, "(");
            for(int i = 0; i < call->argc; i++) {
                compileValUse(src, call->args[i]);
                if(i != call->argc - 1)
                    fkr_writeToStr(src, ", "); 
            }
            fkr_writeToStr(src, ")");
            break;
        }
        case FKR_VAL_STRUCT: {
            fkr_writeToStr(src, "{0}");
            break;
        }
    }

    fkr_writeToStr(src, ";\n");

    if(val->type == FKR_VAL_STRUCT) {
        fkr_valStruct* strukt = (fkr_valStruct*)val;
        for(int i = 0; i < strukt->memCnt; i++) {
            fkr_writeToStr(src, "%s.m%d = ", valName, i);
            compileValUse(src, strukt->members[i]);
            fkr_writeToStr(src, ";\n");
        }
    }
}
;
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

static void writeCFuncDecl(fkr_str* s, fkr_func* fn) {
    fkr_typeFunc* fnType = (fkr_typeFunc*)fn->v.valType;
    compileType(s, fnType->retType);
    fkr_writeToStr(s, " %s(", fn->name.str);
    for(int i = 0; i < fnType->paramCnt; i++) {
        compileType(s, fnType->params[i]);
        fkr_writeToStr(s, " a_%d", i);
        if(i != fnType->paramCnt - 1)
            fkr_writeToStr(s, ", ");
    }
    fkr_writeToStr(s, ")");
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
        fkr_typeFunc* fnType = (fkr_typeFunc*)fn->v.valType;
        writeCFuncDecl(&header, fn);
        fkr_writeToStr(&header, ";\n");

        writeCFuncDecl(&source, fn);
        fkr_writeToStr(&source, " {\n", fn->name.str);
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
