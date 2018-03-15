#ifndef _JV_POOL_H_INCLUDED_
#define _JV_POOL_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JV_PLATFORM_WORD (sizeof(unsigned long))
#define JV_CACHELINE_SIZE (JV_PLATFORM_WORD << 3)


#define JV_OK 0
#define JV_ERROR -1

#define jv_memzero(buf, n) (void *) memset(buf, 0, n)

#ifndef intptr_t
#define intptr_t long
#define uintptr_t unsigned long
#endif

typedef intptr_t jv_int_t;
typedef uintptr_t jv_uint_t;

typedef unsigned char u_char;

typedef struct jv_pool_s jv_pool_t;
typedef struct jv_block_s jv_block_t;
typedef struct jv_lump_s jv_lump_t;

#define jv_align(d, a) (((d) + (a - 1)) & ~(a - 1))

#define JV_BLOCK_DEFAULT_SIZE 0x4000 /* 0x4000 = 1024 * 16 = 16384 */

#define JV_ALLOC_MAX_SIZE 0x7fffffff /* 32bit: 2147483647 64bit: 9223372036854775807 */

struct jv_block_s {
  jv_block_t *next;
  size_t size;
};

/*
 jv_lump_t 采用双向循环链表主要是为了可以高效回收分配的内存
 其实，jv_lump_t可以仅为单向循环链表，但是在回收分配的内存时
 需要先遍历整个内存池
 */
struct jv_lump_s {
  jv_lump_t *prev;
  jv_lump_t *next;
  unsigned size : 31; /* max value: (2<<31)-1 = 2147483647 */
  unsigned used : 1;
};

struct jv_pool_s { /* 20 */
  size_t max;      /* block max size */
  jv_block_t *first;
  jv_block_t *last;
  jv_lump_t *lump;
  jv_lump_t *pos; /* current idle lump's position */
};

jv_pool_t *jv_pool_create(size_t size);

void *jv_pool_alloc(jv_pool_t *pool, size_t size);

void *jv_pool_realloc(jv_pool_t *pool, void *ptr, size_t size);

jv_int_t jv_pool_free(jv_pool_t *pool, void *ptr);

jv_int_t jv_pool_recycle(jv_pool_t *pool, void *ptr);

jv_int_t jv_pool_reset(jv_pool_t *pool);

#define jv_pool_each_lump(pool, lump, i) \
  \
for(lump = (pool)->lump, i = 0; i == 0 || lump != (pool)->lump; i++, lump = lump->prev)

#define jv_pool_each_block(pool, block, i) \
  \
for(block = (pool)->first, i = 0; block != NULL; i++, block = block->next)

void jv_pool_destroy(jv_pool_t *pool);

void jv_pool_dump(jv_pool_t *pool, FILE *fd);

#endif /* _JV_POOL_H_INCLUDED_ */
