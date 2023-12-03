#include <mm_block.h>  // prototypes of functions implemented in this file
#include <stddef.h>    // NULL

/**
 * Points to the first block on the heap.
 */
BlockHeader *heap_blocks;

/**
 * Read the size field from a block header (or footer).
 *
 * @param bp address of the block header (or footer)
 * @return size in bytes
 */
int mm_block_size(BlockHeader *bp) {
    return (*bp) & ~7;  // discard last 3 bits
}

/**
 * Read the allocated bit from a block header (or footer).
 *
 * @param bp address of the block header (or footer)
 * @return allocated bit (either 0 or 1)
 */
int mm_block_allocated(BlockHeader *bp) {
    return (*bp) & 1;   // get last bit
}

/**
 * Write the size and allocated bit of a given block inside its header.
 *
 * @param bp address of the block header
 * @param size size in bytes (must be a multiple of 8)
 * @param allocated either 0 or 1
 */
void mm_block_set_header(BlockHeader *bp, int size, int allocated) {
    *bp = size | allocated;
}

/**
 * Write the size and allocated bit of a given block inside its footer.
 *
 * @param bp address of the block header
 * @param size size in bytes (must be a multiple of 8)
 * @param allocated either 0 or 1
 */
void mm_block_set_footer(BlockHeader *bp, int size, int allocated) {
    char *footer_addr = (char *)bp + mm_block_size(bp) - 4;
    // the footer has the same format as the header
    mm_block_set_header((BlockHeader *)footer_addr, size, allocated);
}

/**
 * Find the payload starting address given the address of a block header.
 *
 * The block header is 4 bytes, so the payload starts after 4 bytes.
 *
 * @param bp address of the block header
 * @return address of the payload for this block
 */
char *mm_block_payload_addr(BlockHeader *bp) {
    return (char *)(bp + 1);
}

/**
 * Find the header address of the previous block on the heap.
 *
 * @param bp address of a block header
 * @return address of the header of the previous block
 */
BlockHeader *mm_block_prev(BlockHeader *bp) {
    // move back by 4 bytes to find the footer of the previous block
    BlockHeader *previous_footer = bp - 1;
    int previous_size = mm_block_size(previous_footer);
    char *previous_addr = (char *)bp - previous_size;
    return (BlockHeader *)previous_addr;
}

/**
 * Find the header address of the next block on the heap.
 *
 * @param bp address of a block header
 * @return address of the header of the next block
 */
BlockHeader *mm_block_next(BlockHeader *bp) {
    int this_size = mm_block_size(bp);

    // TODO: to implement, look at get_prev
    char *next_addr = NULL;
    next_addr = (char *)bp + this_size;
    return (BlockHeader *)next_addr;
}
