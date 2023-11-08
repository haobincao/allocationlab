#include "unity.h"
#include "memlib.h"

#include "mm.h"
#include "mm_block.h"

#include <stdlib.h>

static BlockHeader *new_block(int size) {
    // NOTE: here we are allocating blocks with malloc, but
    // mm.c should allocate them on the heap that you're managing
    return malloc(size);
}

void setUp(void) {

}

void tearDown(void) {

}

void test_mm_block_header(void) {
    BlockHeader *bp = new_block(16);
    mm_block_set_header(bp, 16, 1);
    TEST_ASSERT(mm_block_allocated(bp) == 1);
    TEST_ASSERT(mm_block_size(bp) == 16);
}

void test_mm_block_footer(void) {
    BlockHeader *bp = new_block(16);
    mm_block_set_header(bp, 16, 1);
    mm_block_set_footer(bp, 16, 1);
    // read info from footer at bp+3
    TEST_ASSERT(mm_block_allocated(bp+3) == 1);
    TEST_ASSERT(mm_block_size(bp+3) == 16);
}

void test_mm_block_payload_addr(void) {
    BlockHeader *bp = new_block(16);
    mm_block_set_header(bp, 16, 1);
    TEST_ASSERT((BlockHeader *)mm_block_payload_addr(bp) == bp+1);
}

void test_mm_block_prev_next(void) {
    // allocate a chunk of 48 bytes
    BlockHeader *bp = new_block(48);

    // use the first 32 for a free block
    mm_block_set_header(bp, 32, 0);
    mm_block_set_footer(bp, 32, 0);
    TEST_ASSERT(mm_block_allocated(bp) == 0);    // header
    TEST_ASSERT(mm_block_size(bp) == 32);
    TEST_ASSERT(mm_block_allocated(bp+7) == 0);  // footer
    TEST_ASSERT(mm_block_size(bp+7) == 32);

    // use the next 16 for an allocated block
    BlockHeader *bp_next = mm_block_next(bp);
    TEST_ASSERT(bp_next != NULL);
    mm_block_set_header(bp_next, 16, 1);
    mm_block_set_footer(bp_next, 16, 1);
    TEST_ASSERT(mm_block_allocated(bp_next) == 1);    // header
    TEST_ASSERT(mm_block_size(bp_next) == 16);
    TEST_ASSERT(mm_block_allocated(bp_next+3) == 1);  // footer
    TEST_ASSERT(mm_block_size(bp_next+3) == 16);

    // test next and prev functions
    TEST_ASSERT(mm_block_next(bp) == bp_next);
    TEST_ASSERT(mm_block_prev(bp_next) == bp);
}

int main(void) {
    UNITY_BEGIN();
    mem_init();
    RUN_TEST(test_mm_block_header);
    RUN_TEST(test_mm_block_footer);
    RUN_TEST(test_mm_block_payload_addr);
    RUN_TEST(test_mm_block_prev_next);
    mem_deinit();
    return UNITY_END();
}
