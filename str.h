
#ifndef FKR_STR_H
#define FKR_STR_H

typedef struct {
    char* str;
    int len;
    int cap;
} fkr_str;

fkr_str fkr_makeStr(const char* str);
void fkr_freeStr(fkr_str str);

void fkr_writeToStr(fkr_str* str, const char* fmt, ...);

#endif
