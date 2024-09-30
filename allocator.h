// This software is dual-licensed to the public domain and under the following
// license: you are granted a perpetual, irrevocable license to copy, modify,
// publish, and distribute this file as you see fit.

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifdef ALLOCATOR_IMPLEMENTATION

// linked list style
static struct Block {
    size_t len;
    struct Block *previous, *next;
} Block;

// this aligns the memory size by pointer size
// this is something like if ((n % pointer size) == 0) aligned_address == address else aligned_address == address + adjustment (16 - offset)
#define ALIGN(n) ((n + sizeof(void *) - 1) & ~(sizeof(void *) - 1))
#define BLOCK_LEN (ALIGN(sizeof(Block)))

static struct Block *memory = NULL;

// "block" is our heap, which we will allocate from
int mem_init(void *block, size_t len) {
    if (block == NULL || len < BLOCK_LEN) {
        // not enough size to store a single block
        return 1;
    }

    memory = (struct Block *)block;

    // the size of the block, subtracting the header
    memory->len = len - BLOCK_LEN;
    memory->previous = NULL;
    memory->next = NULL;

    return 0;
}

void *mem_alloc(size_t len) {
    if (memory == NULL) {
        // no more heap!
        return NULL;
    }

    size_t adjusted_len = ALIGN(len + BLOCK_LEN);

    struct Block *next = memory->next;
    struct Block *previous = memory;
    struct Block *current = NULL;

    char *pos = (char *)memory + BLOCK_LEN;

    while (1) {
        if (next != NULL) {
            // the size between both of these blocks in bytes
            size_t size_between = (char *)next - pos;

            if (size_between >= adjusted_len) {
                // we've found our header
                current = (struct Block *)pos;
                break;
            }

            pos = (char *)next + next->len;
            previous = next;
            next = next->next;
        } else {
            // the size between both of these blocks in bytes
            size_t size_between = (char *)memory + memory->len - pos;

            if (size_between < adjusted_len) {
                // not enough space!
                return NULL;
            }

            current = (struct Block *)pos;
            break;
        }
    }

    current->len = adjusted_len;
    current->previous = previous;
    current->next = next;

    // if there is a block found after our newly allocated one, assign it as the previous block of the block found after the recently allocated block
    if (current->next != NULL) {
        current->next->previous = current;
    }
    previous->next = current;

    return (void *)pos + BLOCK_LEN;
}

void mem_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    // get start of block
    struct Block *block = (struct Block *)((char *)ptr - BLOCK_LEN);

    block->previous->next = block->next;
    if (block->previous->next != NULL) {
        block->next->previous = block->previous;
    }
}

void mem_defrag() {
    struct Block *next = memory->next;
    struct Block *previous = memory;
    char *pos = (char *)memory + BLOCK_LEN;

    while (next != NULL) {
        // the size between both of these blocks in bytes
        size_t size_between = (char *)next - pos;

        if (size_between > 0) {
            // shift the next address to the left
            char *src = (char *)next;
            char *dst = pos;

            for (size_t i = 0; i < next->len; i++) {
                dst[i] = src[i];
            }

            previous->next = (struct Block *)pos;
            next = (struct Block *)pos;
        }

        pos = (char *)next + next->len;
        previous = next;
        next = next->next;
    }
}

#endif

#endif  // ALLOCATOR_H