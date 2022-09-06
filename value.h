
#ifndef FKR_VALUE_H
#define FKR_VALUE_H

#include <stdint.h>
#include <stddef.h>
#include "type.h"
#include "str.h"

typedef enum {
    FKR_VAL_CONST,
    FKR_VAL_BINOP,
    FKR_VAL_ALLOC,
    FKR_VAL_GET,
    FKR_VAL_SET,
    FKR_VAL_JUMP,
    FKR_VAL_BRANCH,
    FKR_VAL_RETURN
} fkr_valType;

struct fkr_block;

typedef struct fkr_val {
    fkr_valType type;
    uint64_t sym;
    fkr_type* valType;
    struct fkr_block* block;
    struct fkr_val* next;
} fkr_val;

typedef struct {
    fkr_val v;
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
    } as;
} fkr_valConst;

typedef enum {
    FKR_OP_ADD,
    FKR_OP_SUB,
    FKR_OP_MUL,
    FKR_OP_DIV,
    FKR_OP_EQ
} fkr_op;

typedef struct {
    fkr_val v;
    fkr_val* a;
    fkr_val* b;
    fkr_op op;
} fkr_valBinOp;

typedef struct {
    fkr_val v;
    fkr_val* cnt;
} fkr_valAlloc;

typedef struct {
    fkr_val v;
    fkr_val* ptr;
} fkr_valGet;

typedef struct {
    fkr_val v;
    fkr_val* ptr;
    fkr_val* val;
} fkr_valSet;

typedef struct {
    fkr_val v;
    struct fkr_block* block;
} fkr_valJump;

typedef struct {
    fkr_val v;
    fkr_val* cond;
    struct fkr_block* thenBranch;
    struct fkr_block* elseBranch;
} fkr_valBranch;

typedef struct {
    fkr_val v;
    fkr_val* retVal;
    fkr_type* retType;
} fkr_valReturn;

typedef fkr_val* fkr_valRef;

void fkr_initVal(fkr_val* val);
void fkr_freeVal(fkr_val* val);

#define FKR_MAX_VAL_PTRS 3
extern size_t fkr_valPtrOffsets[][FKR_MAX_VAL_PTRS]; 

#endif
