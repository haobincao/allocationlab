#ifndef __MEMLIB_H__
#define __MEMLIB_H__

void  mem_init(void);
void  mem_deinit(void);
char *mem_sbrk(int incr);
void  mem_reset_brk(void);
char *mem_heap_lo(void);
char *mem_heap_hi(void);
long  mem_heapsize(void);

#endif /* __MEMLIB_H__ */
