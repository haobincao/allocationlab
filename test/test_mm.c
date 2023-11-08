#include "unity.h"
#include "memlib.h"

#include "mm.c"
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

void test_free_coalesce_alloc_alloc(void) {
    BlockHeader *bp1 = new_block(16 + 16 + 16);
    mm_block_set_header(bp1, 16, 1);
    mm_block_set_footer(bp1, 16, 1);
    BlockHeader *bp2 = mm_block_next(bp1);
    TEST_ASSERT(bp2 != NULL);
    mm_block_set_header(bp2, 16, 0);
    mm_block_set_footer(bp2, 16, 0);
    BlockHeader *bp3 = mm_block_next(bp2);
    TEST_ASSERT(bp3 != NULL);
    mm_block_set_header(bp3, 16, 1);
    mm_block_set_footer(bp3, 16, 1);

    mm_list_init();

    // must do no coalescing
    BlockHeader *coalesced = free_coalesce(bp2);
    TEST_ASSERT(coalesced == bp2);
    TEST_ASSERT(mm_list_headp == bp2);
    TEST_ASSERT(mm_list_tailp == bp2);
    TEST_ASSERT(mm_block_size(bp1) == 16);
    TEST_ASSERT(mm_block_allocated(bp1) == 1);
    TEST_ASSERT(mm_block_size(bp1+3) == 16);
    TEST_ASSERT(mm_block_allocated(bp1+3) == 1);
    TEST_ASSERT(mm_block_size(bp2) == 16);
    TEST_ASSERT(mm_block_allocated(bp2) == 0);
    TEST_ASSERT(mm_block_size(bp2+3) == 16);
    TEST_ASSERT(mm_block_allocated(bp2+3) == 0);
    TEST_ASSERT(mm_block_size(bp3) == 16);
    TEST_ASSERT(mm_block_allocated(bp3) == 1);
    TEST_ASSERT(mm_block_size(bp3+3) == 16);
    TEST_ASSERT(mm_block_allocated(bp3+3) == 1);
}

void test_free_coalesce_alloc_free(void) {
    BlockHeader *bp1 = new_block(16 + 16 + 16);
    mm_block_set_header(bp1, 16, 1);
    mm_block_set_footer(bp1, 16, 1);
    *(bp1+1) = 0x01010101;  // payload of 8 bytes
    *(bp1+2) = 0x01010101;
    BlockHeader *bp2 = mm_block_next(bp1);
    TEST_ASSERT(bp2 != NULL);
    mm_block_set_header(bp2, 16, 0);
    mm_block_set_footer(bp2, 16, 0);
    BlockHeader *bp3 = mm_block_next(bp2);
    TEST_ASSERT(bp3 != NULL);
    mm_block_set_header(bp3, 16, 0);
    mm_block_set_footer(bp3, 16, 0);

    mm_list_init();
    mm_list_append(bp3);

    // must coalesce bp2 and bp3, no change to bp1
    BlockHeader *coalesced = free_coalesce(bp2);
    TEST_ASSERT(coalesced == bp2);
    TEST_ASSERT(mm_list_headp == bp2);
    TEST_ASSERT(mm_list_tailp == bp2);
    TEST_ASSERT(mm_block_size(bp1) == 16);
    TEST_ASSERT(mm_block_allocated(bp1) == 1);
    TEST_ASSERT(mm_block_size(bp1+3) == 16);      // footer
    TEST_ASSERT(mm_block_allocated(bp1+3) == 1);
    TEST_ASSERT(*(bp1+1) == 0x01010101);
    TEST_ASSERT(*(bp1+2) == 0x01010101);
    TEST_ASSERT(mm_block_size(bp2) == 32);
    TEST_ASSERT(mm_block_allocated(bp2) == 0);
    TEST_ASSERT(mm_block_size(bp2+7) == 32);      // footer
    TEST_ASSERT(mm_block_allocated(bp2+7) == 0);
}

void test_free_coalesce_free_alloc(void) {
    BlockHeader *bp1 = new_block(16 + 16 + 16);
    mm_block_set_header(bp1, 16, 0);
    mm_block_set_footer(bp1, 16, 0);
    BlockHeader *bp2 = mm_block_next(bp1);
    TEST_ASSERT(bp2 != NULL);
    mm_block_set_header(bp2, 16, 0);
    mm_block_set_footer(bp2, 16, 0);
    BlockHeader *bp3 = mm_block_next(bp2);
    TEST_ASSERT(bp3 != NULL);
    mm_block_set_header(bp3, 16, 1);
    mm_block_set_footer(bp3, 16, 1);
    *(bp3+1) = 0x03030303;  // payload of 8 bytes
    *(bp3+2) = 0x03030303;

    mm_list_init();
    mm_list_append(bp1);

    // must coalesce bp1 and bp2, no change to bp3
    BlockHeader *coalesced = free_coalesce(bp2);
    TEST_ASSERT(coalesced == bp1);
    TEST_ASSERT(mm_list_headp == bp1);
    TEST_ASSERT(mm_list_tailp == bp1);
    TEST_ASSERT(mm_block_size(bp1) == 32);
    TEST_ASSERT(mm_block_allocated(bp1) == 0);
    TEST_ASSERT(mm_block_size(bp1+7) == 32);      // footer
    TEST_ASSERT(mm_block_allocated(bp1+7) == 0);
    TEST_ASSERT(*(bp3+1) == 0x03030303);
    TEST_ASSERT(*(bp3+2) == 0x03030303);
    TEST_ASSERT(mm_block_size(bp3) == 16);
    TEST_ASSERT(mm_block_allocated(bp3) == 1);
    TEST_ASSERT(mm_block_size(bp3+3) == 16);     // footer
    TEST_ASSERT(mm_block_allocated(bp3+3) == 1);
}

void test_free_coalesce_free_free(void) {
    BlockHeader *bp1 = new_block(16 + 16 + 16);
    mm_block_set_header(bp1, 16, 0);
    mm_block_set_footer(bp1, 16, 0);
    BlockHeader *bp2 = mm_block_next(bp1);
    TEST_ASSERT(bp2 != NULL);
    mm_block_set_header(bp2, 16, 0);
    mm_block_set_footer(bp2, 16, 0);
    BlockHeader *bp3 = mm_block_next(bp2);
    TEST_ASSERT(bp3 != NULL);
    mm_block_set_header(bp3, 16, 0);
    mm_block_set_footer(bp3, 16, 0);

    mm_list_init();
    mm_list_append(bp1);
    mm_list_append(bp3);

    // must coalesce bp1, bp2, and bp3
    BlockHeader *coalesced = free_coalesce(bp2);
    TEST_ASSERT(coalesced == bp1);
    TEST_ASSERT(mm_list_headp == bp1);
    TEST_ASSERT(mm_list_tailp == bp1);
    TEST_ASSERT(mm_block_size(bp1) == 48);
    TEST_ASSERT(mm_block_allocated(bp1) == 0);
    TEST_ASSERT(mm_block_size(bp1+11) == 48);      // footer
    TEST_ASSERT(mm_block_allocated(bp1+11) == 0);
}

void test_find_fit(void) {
    int size = 8 * 1024 * 1024;  // 8 MB

    // initialize the heap
    mm_init();
    TEST_ASSERT(mm_block_next(heap_blocks) != NULL);

    // ask for a huge block (not present)
    BlockHeader *bp = find_fit(size);
    TEST_ASSERT(bp == NULL);

    // extend the heap
    BlockHeader *allocated = extend_heap(size);

    // now it should be present
    bp = find_fit(size);
    TEST_ASSERT(bp == allocated);
}

void test_place_small_leftover(void) {
    BlockHeader *bp = new_block(16+8);
    mm_block_set_header(bp, 16+8, 0);
    mm_block_set_footer(bp, 16+8, 0);
    mm_list_init();
    mm_list_append(bp);

    // leftover too small (8 bytes), use all
    BlockHeader *placed = place(bp, 16);
    TEST_ASSERT(placed == bp);
    TEST_ASSERT(mm_block_size(placed) == 16+8);
    TEST_ASSERT(mm_block_allocated(placed) == 1);
    TEST_ASSERT(mm_list_headp == NULL);
    TEST_ASSERT(mm_list_tailp == NULL);
}

void test_place_small_leftover_bis(void) {
    BlockHeader *bp = new_block(160+8);
    mm_block_set_header(bp, 160+8, 0);
    mm_block_set_footer(bp, 160+8, 0);
    mm_list_init();
    mm_list_append(bp);

    BlockHeader *placed = place(bp, 160);
    TEST_ASSERT(placed == bp);
    TEST_ASSERT(mm_block_size(placed) == 160+8);
    TEST_ASSERT(mm_block_allocated(placed) == 1);
    TEST_ASSERT(mm_list_headp == NULL);
    TEST_ASSERT(mm_list_tailp == NULL);
}

void test_place_large_leftover(void) {
    BlockHeader *bp = new_block(16+16);
    mm_block_set_header(bp, 16+16, 0);
    mm_block_set_footer(bp, 16+16, 0);
    mm_list_init();
    mm_list_append(bp);

    BlockHeader *placed = place(bp, 16);
    TEST_ASSERT(placed != NULL);
    TEST_ASSERT(mm_block_size(placed) == 16);
    TEST_ASSERT(mm_block_allocated(placed) == 1);
    TEST_ASSERT(mm_list_headp != placed);
    TEST_ASSERT(mm_list_headp == mm_list_tailp);
    TEST_ASSERT(mm_list_headp == mm_block_next(placed) || mm_list_headp == mm_block_prev(placed));
    TEST_ASSERT(mm_block_size(mm_list_headp) == 16);
    TEST_ASSERT(mm_block_allocated(mm_list_headp) == 0);
}

void test_malloc_free(void) {
    mm_init();
    int *p1 = mm_malloc(8);
    TEST_ASSERT(p1 != NULL);
    *p1 = 0x01010101;
    *(p1+1) = 0x01010101;

    int *p2 = mm_malloc(16);
    TEST_ASSERT(p2 != NULL);
    *p2 = 0x02020202;
    *(p2+1) = 0x02020202;
    *(p2+2) = 0x02020202;
    *(p2+3) = 0x02020202;

    // check that p2's payload is not overlapping with p1's
    TEST_ASSERT(*p1 == 0x01010101);
    TEST_ASSERT(*(p1+1) == 0x01010101);

    // check that freeing p1 doesn't affect p2
    mm_free(p1);
    TEST_ASSERT(*p2 == 0x02020202);
    TEST_ASSERT(*(p2+1) == 0x02020202);
    TEST_ASSERT(*(p2+2) == 0x02020202);
    TEST_ASSERT(*(p2+3) == 0x02020202);

    // free p2
    mm_free(p2);
}

void test_malloc_realloc_free(void) {
    int *p1 = mm_malloc(8);
    TEST_ASSERT(p1 != NULL);
    *p1 = 0x01010101;
    *(p1+1) = 0x01010101;

    int *p2 = mm_malloc(16);
    TEST_ASSERT(p2 != NULL);
    *p2 = 0x02020202;
    *(p2+1) = 0x02020202;
    *(p2+2) = 0x02020202;
    *(p2+3) = 0x02020202;

    p1 = mm_realloc(p1, 32);
    TEST_ASSERT(p1 != NULL);

    // check that p1's payload was copied over
    TEST_ASSERT(*p1 == 0x01010101);
    TEST_ASSERT(*(p1+1) == 0x01010101);

    // write on the rest of p1
    *(p1+2) = 0x01010101;
    *(p1+3) = 0x01010101;
    *(p1+4) = 0x01010101;
    *(p1+5) = 0x01010101;
    *(p1+6) = 0x01010101;
    *(p1+7) = 0x01010101;

    // check p1's new payload doesn't overlap with p2's
    TEST_ASSERT(*p2 == 0x02020202);
    TEST_ASSERT(*(p2+1) == 0x02020202);
    TEST_ASSERT(*(p2+2) == 0x02020202);
    TEST_ASSERT(*(p2+3) == 0x02020202);

    // free p1, p2
    mm_free(p1);
    mm_free(p2);
}

int main(void) {
    UNITY_BEGIN();
    mem_init();
    RUN_TEST(test_free_coalesce_alloc_alloc);
    RUN_TEST(test_free_coalesce_alloc_free);
    RUN_TEST(test_free_coalesce_free_alloc);
    RUN_TEST(test_free_coalesce_free_free);
    RUN_TEST(test_find_fit);
    RUN_TEST(test_place_small_leftover);
    RUN_TEST(test_place_small_leftover_bis);
    RUN_TEST(test_place_large_leftover);
    RUN_TEST(test_malloc_free);
    RUN_TEST(test_malloc_realloc_free);
    mem_deinit();
    return UNITY_END();
}
