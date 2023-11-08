#include "unity.h"
#include "memlib.h"

#include "mm.h"
#include "mm_list.h"

#include <stdlib.h>

static BlockHeader *new_block() {
    // NOTE: here we are allocating blocks with malloc, but
    // mm.c should allocate them on the heap that you're managing
    BlockHeader *bp = malloc(16);
    *(bp+1) = 0x03030303;
    *(bp+2) = 0x04040404;
    return bp;
}

void setUp(void) {
    mm_list_init();
}

void tearDown(void) {

}

void test_append_empty(void) {
    TEST_ASSERT(mm_list_headp == NULL);
    TEST_ASSERT(mm_list_tailp == NULL);
    BlockHeader *b1 = new_block();
    mm_list_append(b1);
    TEST_ASSERT(mm_list_headp == b1);
    TEST_ASSERT(mm_list_next(b1) == NULL);
    TEST_ASSERT(mm_list_prev(b1) == NULL);
    TEST_ASSERT(mm_list_tailp == b1);
}

void test_prepend_empty(void) {
    TEST_ASSERT(mm_list_headp == NULL);
    TEST_ASSERT(mm_list_tailp == NULL);
    BlockHeader *b1 = new_block();
    mm_list_prepend(b1);
    TEST_ASSERT(mm_list_headp == b1);
    TEST_ASSERT(mm_list_next(b1) == NULL);
    TEST_ASSERT(mm_list_prev(b1) == NULL);
    TEST_ASSERT(mm_list_tailp == b1);
}

void test_append_nonempty(void) {
    BlockHeader *b1 = new_block();
    BlockHeader *b2 = new_block();
    mm_list_append(b1);
    mm_list_append(b2);
    TEST_ASSERT(mm_list_headp == b1);
    TEST_ASSERT(mm_list_prev(b1) == NULL);
    TEST_ASSERT(mm_list_next(b1) == b2);
    TEST_ASSERT(mm_list_prev(b2) == b1);
    TEST_ASSERT(mm_list_next(b2) == NULL);
    TEST_ASSERT(mm_list_tailp == b2);
}

void test_prepend_nonempty(void) {
    BlockHeader *b1 = new_block();
    BlockHeader *b2 = new_block();
    mm_list_prepend(b1);
    mm_list_prepend(b2);

    TEST_ASSERT(mm_list_headp == b2);
    TEST_ASSERT(mm_list_prev(b2) == NULL);
    TEST_ASSERT(mm_list_next(b2) == b1);
    TEST_ASSERT(mm_list_prev(b1) == b2);
    TEST_ASSERT(mm_list_next(b1) == NULL);
    TEST_ASSERT(mm_list_tailp == b1);
}

void test_remove_single(void) {
    BlockHeader *b1 = new_block();
    mm_list_append(b1);
    TEST_ASSERT(mm_list_headp == b1);
    TEST_ASSERT(mm_list_tailp == b1);
    mm_list_remove(b1);
    TEST_ASSERT(mm_list_headp == NULL);
    TEST_ASSERT(mm_list_tailp == NULL);
}

void test_remove_head(void) {
    BlockHeader *b1 = new_block();
    BlockHeader *b2 = new_block();
    mm_list_append(b1);
    mm_list_append(b2);
    mm_list_remove(b1);
    TEST_ASSERT(mm_list_headp == b2);
    TEST_ASSERT(mm_list_prev(b2) == NULL);
    TEST_ASSERT(mm_list_next(b2) == NULL);
    TEST_ASSERT(mm_list_tailp == b2);
}

void test_remove_tail(void) {
    BlockHeader *b1 = new_block();
    BlockHeader *b2 = new_block();
    mm_list_append(b1);
    mm_list_append(b2);
    mm_list_remove(b2);
    TEST_ASSERT(mm_list_headp == b1);
    TEST_ASSERT(mm_list_prev(b1) == NULL);
    TEST_ASSERT(mm_list_next(b1) == NULL);
    TEST_ASSERT(mm_list_tailp == b1);
}

void test_remove_middle(void) {
    BlockHeader *b1 = new_block();
    BlockHeader *b2 = new_block();
    BlockHeader *b3 = new_block();
    mm_list_append(b1);
    mm_list_append(b2);
    mm_list_append(b3);
    mm_list_remove(b2);
    TEST_ASSERT(mm_list_headp == b1);
    TEST_ASSERT(mm_list_prev(b1) == NULL);
    TEST_ASSERT(mm_list_next(b1) == b3);
    TEST_ASSERT(mm_list_prev(b3) == b1);
    TEST_ASSERT(mm_list_next(b3) == NULL);
    TEST_ASSERT(mm_list_tailp == b3);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_append_empty);
    RUN_TEST(test_append_nonempty);
    RUN_TEST(test_prepend_empty);
    RUN_TEST(test_prepend_nonempty);
    RUN_TEST(test_remove_single);
    RUN_TEST(test_remove_head);
    RUN_TEST(test_remove_tail);
    RUN_TEST(test_remove_middle);
    return UNITY_END();
}
