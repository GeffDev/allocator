#include <stdio.h>

#define ALLOCATOR_IMPLEMENTATION
#include "allocator.h"

static unsigned char mem_pool[0x1000];

int main() {
    printf("aligned size test (3): %lu\n", ALIGN(3));

    mem_init(mem_pool, 0x1000);
    unsigned char *allocated_array = mem_alloc(0x100);
    unsigned char *allocated_array1 = mem_alloc(0x100);
    unsigned char *allocated_array2 = mem_alloc(0x100);

    mem_free(allocated_array);
    mem_free(allocated_array1);
    mem_free(allocated_array2);
}
