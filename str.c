
#include "str.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

fkr_str fkr_makeStr(const char* str) {
    fkr_str s;
    s.len = strlen(str);
    s.cap = 8;
    while(s.len + 1 > s.cap)
        s.cap *= 2;
    s.str = malloc(s.cap);
    memcpy(s.str, str, s.len);
    s.str[s.len] = '\0';
    return s;
}

void fkr_freeStr(fkr_str str) {
    free(str.str);
}



void fkr_writeToStr(fkr_str* str, const char* fmt, ...) {

    va_list args;
    va_start(args, fmt);    
    va_list argsCpy;
    va_copy(argsCpy, args);

    if(str->str == NULL) {
        str->str = malloc(8);
        str->str[0] = '\0';
        str->len = 0;
        str->cap = 8;
    }

    int fmtLen = vsnprintf(NULL, 0, fmt, args);
    str->len += fmtLen;

    if(str->len + 1 > str->cap) {
        while(str->len + 1 > str->cap)
            str->cap *= 2;
        str->str = realloc(str->str, str->cap);
    }
    
    vsprintf(str->str + str->len - fmtLen, fmt, argsCpy);
    str->str[str->len] = '\0';

    va_end(args);
    va_end(argsCpy);

}
