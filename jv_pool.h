#ifndef _JV_POOL_H_INCLUDED_
#define _JV_POOL_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __x86_64__
#define JV_WORD_SIZE 64
#else
#define JV_WORD_SIZE 32
#endif

#define JV_OK 0
#define JV_ERROR -1

#define jv_memzero(buf, n) (void *) memset(buf, 0, n)
#define jv_memset(buf, c, n) (void *) memset(buf, c, n)

#ifndef intptr_t
#define intptr_t long
#define uintptr_t unsigned long
#endif

typedef intptr_t jv_int_t;
typedef uintptr_t jv_uint_t;

typedef unsigned char u_char;

#define JV_ALLOC_MIN_SIZE 256

#define JV_ALLOC_DEFAULT_SIZE 0x4000 /* 0x4000  = 1024 * 16 = 16384 */

/**
 *  (2<<31) = 2147483648
 *  (2<<63) = 9223372036854775808s
 **/
#define JV_ALLOC_MAX_SIZE (2147483648 - JV_ALLOC_MIN_SIZE)

typedef struct jv_pool_s jv_pool_t;
typedef struct jv_block_s jv_block_t;
typedef struct jv_lump_s jv_lump_t;

#define jv_align(d, a) (((d) + (a - 1)) & ~(a - 1))

struct jv_block_s {
  jv_block_t *next;
  size_t size;
};

struct jv_lump_s {
  jv_lump_t *prev;
  jv_lump_t *next;
  unsigned size : 31;
  unsigned used : 1;
};

struct jv_pool_s {
  size_t max; /* block max size */
  jv_block_t *first;
  jv_block_t *last;
  jv_lump_t *lump;
  jv_lump_t *pos; /* current idle lump's position */
};

jv_pool_t *jv_pool_create(size_t size);

void *jv_pool_alloc(jv_pool_t *pool, size_t size);

void *jv_pool_realloc(jv_pool_t *pool, void *ptr, size_t size);

jv_int_t jv_pool_free(jv_pool_t *pool, void *ptr);

size_t jv_pool_sizeof(jv_pool_t *pool, void *ptr);

jv_int_t jv_pool_exist(jv_pool_t *pool, void *ptr);

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
