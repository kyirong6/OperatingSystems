#include <stdbool.h>
#ifndef BBUFF_H
#define BBUFF_H
//typedef bool _Bool;

void bbuff_init(void);
void bbuff_blocking_insert(void* item);
void* bbuff_blocking_extract(void);
_Bool bbuff_is_empty(void);

#endif