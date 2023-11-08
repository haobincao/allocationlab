#ifndef __MM_LIST_H__
#define __MM_LIST_H__

#include <mm_block.h>  // BlockHeader

/**
 * Pointers to the list head and tail (blocks on the heap).
 */
extern BlockHeader *mm_list_headp;
extern BlockHeader *mm_list_tailp;

void mm_list_init();
void mm_list_prepend(int *bp);
void mm_list_append(int *bp);
void mm_list_remove(int *bp);
BlockHeader *mm_list_prev(BlockHeader *bp);
BlockHeader *mm_list_next(BlockHeader *bp);

#endif /* __MM_LIST_H__ */
