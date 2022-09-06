
#ifndef FKR_VERIFY_H
#define FKR_VERIFY_H

#include "str.h"
#include "pkg.h"
#include <stdbool.h>

typedef struct {
    bool error;
    fkr_str msg;
} fkr_error;

fkr_error fkr_verifyPkg(fkr_pkgRef pkg);

#endif
