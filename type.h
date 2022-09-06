
#ifndef FKR_TYPE_H
#define FKR_TYPE_H

#include <stdbool.h>

typedef enum {
    FKR_TYPE_ERR,
    FKR_TYPE_VOID,
    FKR_TYPE_I8,
    FKR_TYPE_I16,
    FKR_TYPE_I32,
    FKR_TYPE_I64,
    FKR_TYPE_U8,
    FKR_TYPE_U16,
    FKR_TYPE_U32,
    FKR_TYPE_U64,
    FKR_TYPE_F32,
    FKR_TYPE_F64,
    FKR_TYPE_PTR,
    FKR_TYPE_FUNC
} fkr_typeType;

typedef struct {
    const char* name;
    const char* cName;
} fkr_typeName;

extern fkr_typeName fkr_typeNames[]; 

typedef struct fkr_type {
    fkr_typeType type;
    struct fkr_type* nextType;
} fkr_type;

typedef fkr_type* fkr_typeRef;

typedef struct {
    fkr_type t;
    fkr_type* elemType;
} fkr_typePtr;

fkr_typeRef fkr_voidType();
fkr_typeRef fkr_i8Type();
fkr_typeRef fkr_i16Type();
fkr_typeRef fkr_i32Type();
fkr_typeRef fkr_i64Type();
fkr_typeRef fkr_u8Type();
fkr_typeRef fkr_u16Type();
fkr_typeRef fkr_u32Type();
fkr_typeRef fkr_u64Type();
fkr_typeRef fkr_f32Type();
fkr_typeRef fkr_f64Type();

fkr_typeType fkr_getTypeType(fkr_typeRef type);
bool fkr_isPrimitiveType(fkr_typeRef type);
bool fkr_isIntegralType(fkr_typeRef type);
bool fkr_isNumericType(fkr_typeRef type);
bool fkr_isPtrType(fkr_typeRef type);

#endif
