
#include "type.h"
#include <stdlib.h>

fkr_typeName fkr_typeNames[] = {
    [FKR_TYPE_ERR] = {"<ERROR>", "<ERROR>"},
    [FKR_TYPE_VOID] = {"void", "void"},
    [FKR_TYPE_I8]  = {"i8",  "int8_t"},
    [FKR_TYPE_I16] = {"i16", "int16_t"},
    [FKR_TYPE_I32] = {"i32", "int32_t"},
    [FKR_TYPE_I64] = {"i64", "int64_t"},
    [FKR_TYPE_U8]  = {"u8",  "uint8_t"},
    [FKR_TYPE_U16] = {"u16", "uint16_t"},
    [FKR_TYPE_U32] = {"u32", "uint32_t"},
    [FKR_TYPE_U64] = {"u64", "uint64_t"},
    [FKR_TYPE_F32] = {"f32", "float"},
    [FKR_TYPE_F64] = {"f64", "double"}
};

fkr_typeRef fkr_voidType() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_VOID;
}

fkr_typeRef fkr_i8Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_I8;
}

fkr_typeRef fkr_i16Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_I16;
}

fkr_typeRef fkr_i32Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_I32;
}

fkr_typeRef fkr_i64Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_I64;
}

fkr_typeRef fkr_u8Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_U8;
}

fkr_typeRef fkr_u16Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_U16;
}

fkr_typeRef fkr_u32Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_U32;
}

fkr_typeRef fkr_u64Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_U64;
}

fkr_typeRef fkr_f32Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_F32;
}

fkr_typeRef fkr_f64Type() {
    return (fkr_typeRef)(intptr_t)FKR_TYPE_F64;
}

fkr_typeType fkr_getTypeType(fkr_typeRef type) {
    uint64_t ptrAsEnum = (uint64_t)(intptr_t)type;
    if(ptrAsEnum >= FKR_TYPE_ERR && ptrAsEnum <= FKR_TYPE_F64) 
        return ptrAsEnum;
    return type->type;
}

bool fkr_isPrimitiveType(fkr_typeRef type) {
    fkr_typeType t = fkr_getTypeType(type);
    return t >= FKR_TYPE_ERR && t <= FKR_TYPE_F64;
}

bool fkr_isIntegralType(fkr_typeRef type) {
    fkr_typeType t = fkr_getTypeType(type);
    return t >= FKR_TYPE_I8 && t <= FKR_TYPE_U64;
}

bool fkr_isNumericType(fkr_typeRef type) {
    fkr_typeType t = fkr_getTypeType(type);
    return t >= FKR_TYPE_I8 && t <= FKR_TYPE_F64;
}

bool fkr_isPtrType(fkr_typeRef type) {
    fkr_typeType t = fkr_getTypeType(type);
    return t == FKR_TYPE_PTR;
}

void fkr_freeType(fkr_type* t) {
    if(t->type == FKR_TYPE_FUNC) {
        fkr_typeFunc* func = (fkr_typeFunc*)t;
        free(func->params);
    }
}
