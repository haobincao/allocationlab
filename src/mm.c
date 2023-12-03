#include "mm.h"        // prototypes of functions implemented in this file
#include "mm_list.h"   // "mm_list_..."  functions -- to manage explicit free list
#include "mm_block.h"  // "mm_block_..." functions -- to manage blocks on the heap
#include "memlib.h"    // mem_sbrk -- to extend the heap
#include <string.h>    // memcpy -- to copy regions of memory

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

/**
 * Mark a block as free, coalesce with contiguous free blocks on the heap, add
 * the coalesced block to the free list.
 *
 * @param bp address of the block to mark as free
 * @return the address of the coalesced block
 */
static BlockHeader *free_coalesce(BlockHeader *bp) {

    // mark block as free
    int size = mm_block_size(bp);
    mm_block_set_header(bp, size, 0);
    mm_block_set_footer(bp, size, 0);

    // check whether contiguous blocks are allocated
    int prev_alloc = mm_block_allocated(mm_block_prev(bp));
    int next_alloc = mm_block_allocated(mm_block_next(bp));

    if (prev_alloc && next_alloc) {
        // TODO: add bp to free list
        mm_list_prepend(bp);
        return bp;

    } else if (prev_alloc && !next_alloc) {
        // TODO: remove next block from free list
        // TODO: add bp to free list
        // TODO: coalesce with next block
        size += mm_block_size(mm_block_next(bp));
        mm_list_remove(mm_block_next(bp));
        mm_list_prepend(bp);
        mm_block_set_header(bp, size, 0);
        mm_block_set_footer(bp, size, 0);
        return bp;

    } else if (!prev_alloc && next_alloc) {
        // TODO: coalesce with previous block
        size += mm_block_size(mm_block_prev(bp));
        mm_block_set_header(mm_block_prev(bp),size, 0);
        mm_block_set_footer(mm_block_prev(bp),size, 0);
        return mm_block_prev(bp);

    } else {
        // TODO: remove next block from free list
        // TODO: coalesce with previous and next block
        size += mm_block_size(mm_block_next(bp)) + mm_block_size(mm_block_prev(bp));
        mm_list_remove(mm_block_next(bp));
        mm_block_set_header(mm_block_prev(bp),size, 0);
        mm_block_set_footer(mm_block_prev(bp),size, 0);
        return mm_block_prev(bp);
    }
}

/**
 * Allocate a free block of `size` byte (multiple of 8) on the heap.
 *
 * @param size number of bytes to allocate (a multiple of 8)
 * @return pointer to the header of the allocated block
 */
static BlockHeader *extend_heap(int size) {

    // bp points to the beginning of the new block
    char *bp = mem_sbrk(size);
    if ((long)bp == -1)
        return NULL;

    // write header over old epilogue, then the footer
    BlockHeader *old_epilogue = (BlockHeader *)bp - 1;
    mm_block_set_header(old_epilogue, size, 0);
    mm_block_set_footer(old_epilogue, size, 0);

    // write new epilogue
    mm_block_set_header(mm_block_next(old_epilogue), 0, 1);

    // merge new block with previous one if possible
    return free_coalesce(old_epilogue);
}

int mm_init(void) {

    // init list of free blocks
    mm_list_init();

    // create empty heap of 4 x 4-byte words
    char *new_region = mem_sbrk(16);
    if ((long)new_region == -1)
        return -1;

    heap_blocks = (BlockHeader *)new_region;
    mm_block_set_header(heap_blocks, 0, 0);      // skip 4 bytes for alignment
    mm_block_set_header(heap_blocks + 1, 8, 1);  // allocate a block of 8 bytes as prologue
    mm_block_set_footer(heap_blocks + 1, 8, 1);
    mm_block_set_header(heap_blocks + 3, 0, 1);  // epilogue (size 0, allocated)
    heap_blocks += 1;                            // point to the prologue header

    // TODO: extend heap with an initial heap size
    extend_heap(64);

    return 0;
}

void mm_free(void *bp) {
    // TODO: move back 4 bytes to find the block header, then free block
    if (bp == NULL) {
        return; 
    }

    BlockHeader *blockHeader = (BlockHeader *)((char *)bp - 4);
    free_coalesce(blockHeader);
}

/**
 * Find a free block with size greater or equal to `size`.
 *
 * @param size minimum size of the free block
 * @return pointer to the header of a free block or `NULL` if free blocks are
 *         all smaller than `size`.
 */
static BlockHeader *find_fit(int size) {
    // TODO: implement
    BlockHeader *bp = mm_list_headp;
    while (bp != NULL) {
        if (mm_block_size(bp) >= size) {
            return bp;
        }
        bp = mm_list_next(bp);
    }
    return NULL;
}

/**
 * Allocate a block of `size` bytes inside the given free block `bp`.
 *
 * @param bp pointer to the header of a free block of at least `size` bytes
 * @param size bytes to assign as an allocated block (multiple of 8)
 * @return pointer to the header of the allocated block
 */
static BlockHeader *place(BlockHeader *bp, int size) {
    int old_size = mm_block_size(bp);
    int new_size = old_size - size;

    if (new_size >= 16) {
        if (size >= 75) {
            mm_block_set_header(bp, new_size, 1);
            mm_block_set_footer(bp, new_size, 1);
            BlockHeader *new_bp = mm_block_next(bp);
            mm_block_set_header(new_bp, size, 1);
            mm_block_set_footer(new_bp, size, 1);
            mm_block_set_header(bp, new_size, 0);
            mm_block_set_footer(bp, new_size, 0);
            return new_bp;
        }
        else {
        mm_block_set_header(bp, size, 1);
        mm_block_set_footer(bp, size, 1);
        BlockHeader *new_bp = mm_block_next(bp);
        mm_list_prepend(new_bp);
        mm_block_set_header(new_bp, new_size, 0);
        mm_block_set_footer(new_bp, new_size, 0);
        }
    }
    else {
        mm_block_set_header(bp, old_size, 1);
        mm_block_set_footer(bp, old_size, 1);
    }

    mm_list_remove(bp);

    return bp;
}

/**
 * Compute the required block size (including space for header/footer) from the
 * requested payload size.
 *
 * @param payload_size requested payload size
 * @return a block size including header/footer that is a multiple of 8
 */
static int required_block_size(int payload_size) {
    payload_size += 8;                    // add 8 for for header/footer
    return ((payload_size + 7) / 8) * 8;  // round up to multiple of 8
}

void *mm_malloc(size_t size) {
    // ignore spurious requests
    if (size == 0)
        return NULL;

    int required_size = required_block_size(size);

    // TODO: find a free block or extend heap
    // TODO: allocate and return pointer to payload
    BlockHeader* temp = find_fit(required_size);
    while (temp == NULL) {
        int tempp;
        if (required_size > 512) {
            tempp = required_size;
        }
        else {
            tempp = 512;
        }
        extend_heap(tempp);
        temp = find_fit(required_size);
    }
    BlockHeader* result = place(temp,required_size);
    return (BlockHeader *)((char *)result + 4);
}

void *mm_realloc(void *ptr, size_t size) {
    // Equivalent to malloc if ptr is NULL
    if (ptr == NULL) {
        return mm_malloc(size);
    }

    // Equivalent to free if size is 0
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    BlockHeader *block_header = (BlockHeader *)((char *)ptr - 4);
    size_t old_size = mm_block_size(block_header) - 8;

    if (size <= old_size) {
        return ptr;
    }

    BlockHeader *next_block = mm_block_next(block_header);
    if (!mm_block_allocated(next_block)) {
        size_t combined_size = mm_block_size(block_header) + mm_block_size(next_block);
        if (combined_size - 8 >= size) {
            mm_list_remove(next_block);
            mm_block_set_header(block_header, combined_size, 1);
            mm_block_set_footer(block_header, combined_size, 1);
            return ptr;
        }
    }

    void *new_ptr = mm_malloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    int tempp;
    if (old_size > size) {
        tempp = size;
    }
    else {
        tempp = old_size;
    }
    memcpy(new_ptr, ptr, tempp);
    mm_free(ptr);
    
    return new_ptr;
}

