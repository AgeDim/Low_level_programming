#include "tests.h"
#include <inttypes.h>
#include <errno.h>
#include <string.h>

static void print_initial_heap() {
    debug("Initial heap state\n");
    debug_heap(stderr, HEAP_START);
}

static void print_line() {
    debug("\n-------------------------------------------------------\n\n");
}

static void test1() {
    debug("Test 1: allocate one block\n");

    print_initial_heap();

    debug("Allocating memory for 10 int32_t\n");
    int32_t* arr = _malloc(sizeof(int32_t) * 10);
    debug("Content address: %p\n", (void*)arr);

    debug_heap(stderr, HEAP_START);
    for (size_t i = 0; i < 10; ++i) {
        arr[i] = 123 + i;
    }

    debug("Content of array: ");
    for (size_t i = 0; i < 10; ++i) {
        debug("%" PRId32 " ", arr[i]);
    }
    debug("\n");
    _free(arr);
    debug("Memory freed\n");
    debug_heap(stderr, HEAP_START);
    print_line();
}

static void test2() {
    debug("Test 2: allocate blocks and free one\n");

    print_initial_heap();

    debug("Allocating 5 int64_t\n");
    int64_t* arr = _malloc(sizeof(int64_t) * 5);
    debug("Alocating 5 uint32_t\n");
    int32_t* lower_arr = _malloc(sizeof(uint32_t) * 5);
    debug("Alocating 12 chars\n");
    char* string = _malloc(12);

    debug_heap(stderr, HEAP_START);

    debug("One block with uint32_t freed\n");
    _free(lower_arr);
    debug_heap(stderr, HEAP_START);


    debug("All blocks freed\n");
    _free(string);
    _free(arr);
    debug_heap(stderr, HEAP_START);

    print_line();
}

static void test3() {
    debug("Test 3: allocate blocks and free 2 of them\n");

    print_initial_heap();

    debug("Allocating 5 int64_t\n");
    int64_t* arr = _malloc(sizeof(int64_t) * 5);
    debug("Alocating 5 uint32_t\n");
    int32_t* lower_arr = _malloc(sizeof(uint32_t) * 5);
    debug("Alocating 12 chars\n");
    char* string = _malloc(12);

    debug_heap(stderr, HEAP_START);

    debug("Freeing 2 block with uint32_t and int64_t\n");
    _free(lower_arr);
    _free(arr);

    debug_heap(stderr, HEAP_START);

    _free(string);

    print_line();

}

static void test4() {
    debug("Test 4: allocate block to make heap grow 'smoothly'\n");

    print_initial_heap();

    debug("Allocating 5 int64_t\n");
    int64_t* arr = _malloc(sizeof(int64_t) * 5);
    debug_heap(stderr, HEAP_START);
    debug("Allocating huge string - 9000 chars\n");
    char* huge = _malloc(9000);
    debug_heap(stderr, HEAP_START);
    _free(arr);
    _free(huge);
    print_line();
}

static void* map_pages(void const* addr, size_t length, int additional_flags) {
    return mmap((void*)addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags, 0, 0);
}

static void test5() {
    debug("Test 5: allocate block to grow heap in random free position\n");

    print_initial_heap();

    debug("Allocating huge array of 17000 chars\n");
    int64_t* arr = _malloc(sizeof(int64_t) * 10);
    char* huge = _malloc(17000);
    debug("heap grows 'smoothly'\n");
    debug_heap(stderr, HEAP_START);

    debug("Manually taking 2048 bytes after the heap\n");

    void* result = map_pages(huge + 21000, 2048, 0);
    if (result == MAP_FAILED) {
        debug("unable to manually allocate memory\n");
        fprintf(stderr, "%s\n", strerror(errno));
    }

    debug("Allocating another huge array\n");
    char* new_reg = _malloc(4000);
    debug_heap(stderr, HEAP_START);
    debug("New region doesn't continue heap\n");


    _free(arr);
    _free(huge);
    _free(new_reg);

    print_line();
}

void test_all() {
    heap_init(INITIAL_HEAP_SIZE);
    test1();
    test2();
    test3();
    test4();
    test5();
}
