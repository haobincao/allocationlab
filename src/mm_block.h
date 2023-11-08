#ifndef __MM_BLOCK_H__
#define __MM_BLOCK_H__

/**
 * A block header uses 4 bytes for:
 * - a block size, multiple of 8 (so, the last 3 bits are always 0's)
 * - an allocated bit (stored as LSB, since the last 3 bits are not needed)
 *
 * A block footer has the same format.
 * Check Figure 9.48(a) in the textbook.
 */
typedef int BlockHeader;

/**
 * Points to the first block on the heap.
 */
extern BlockHeader *heap_blocks;

int mm_block_size(BlockHeader *bp);
int mm_block_allocated(BlockHeader *bp);
void mm_block_set_header(BlockHeader *bp, int size, int allocated);
void mm_block_set_footer(BlockHeader *bp, int size, int allocated);
char *mm_block_payload_addr(BlockHeader *bp);
BlockHeader *mm_block_prev(BlockHeader *bp);
BlockHeader *mm_block_next(BlockHeader *bp);

#endif /* __MM_BLOCK_H__ */
