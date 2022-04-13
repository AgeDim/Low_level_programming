#ifndef TESTS_H
#define TESTS_H

#include "./mem/mem.h"

#define INITIAL_HEAP_SIZE 2 * 4096

struct block_header;
void test_all();
void debug(const char* fmt, ...);

#endif
