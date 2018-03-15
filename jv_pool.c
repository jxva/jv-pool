#include <jv_pool.h>

/* static pthread_mutex_t jv_pool_lock = PTHREAD_MUTEX_INITIALIZER; */

static void *jv_pool_slb(jv_pool_t *pool, size_t size);

static void *jv_pool_alloc_block(jv_pool_t *pool, size_t size);

static void *jv_pool_alloc_large(jv_pool_t *pool, size_t size);

jv_pool_t *jv_pool_create(size_t size) {
  jv_pool_t *pool;
  jv_block_t *block;
  jv_lump_t *lump;
  u_char *cp;

  if (size < JV_BLOCK_DEFAULT_SIZE) {
    size = JV_BLOCK_DEFAULT_SIZE;
  }

  size = jv_align(size, JV_PLATFORM_WORD);

  cp = malloc(size + sizeof(jv_block_t) + sizeof(jv_pool_t) + sizeof(jv_lump_t));
  if (cp == NULL) {
    printf("malloc failed in jv_pool_create()\n");
    return (jv_pool_t *) 0;
  }

  printf("created memory pool, address: %lu, default size: %lu\n", (jv_uint_t) cp, size);

  block = (jv_block_t *) cp;
  pool = (jv_pool_t *) (cp + sizeof(jv_block_t));
  lump = (jv_lump_t *) (cp + sizeof(jv_block_t) + sizeof(jv_pool_t));

  /* (void) pthread_mutex_lock(&jv_pool_lock); */

  block->size = size;
  block->next = NULL;

  pool->max = size;
  pool->first = pool->last = block;
  pool->pos = pool->lump = lump->next = lump->prev = lump;

  lump->size = size;
  lump->used = 0;

  /* (void) pthread_mutex_unlock(&jv_pool_lock); */

  return pool;
}

static void *jv_pool_slb(jv_pool_t *pool, size_t size) {
  if (size > JV_ALLOC_MAX_SIZE) {
    printf("memory allocate failed, max memory size: %d, allocate size: %lu\n", JV_ALLOC_MAX_SIZE, (jv_uint_t)size);
    return NULL;
  }

  if (size <= pool->max) { /* 从 lump->size <= pool->max 中查找 */
    jv_lump_t *p;
    /* jv_uint_t i; */
    size = jv_align(size, JV_PLATFORM_WORD);

    /*for (p = pool->pos, i = 0; i == 0 || p != pool->pos; i++, p = p->next) { */
    p = pool->pos;
    do {
      if (p->used == 0 && p->size <= pool->max && p->size >= size) { /* first fit */
        if (p->size <= size + 2 * sizeof(jv_lump_t)) {               /* best fit  */
          p->used = 1;
          printf("allocate lump memory address using best fit: %lu, size:%lu\n", (jv_uint_t)(void *) (p + 1), (jv_uint_t) size);
          return (void *) (p + 1);
        } else { /* 将所找的Lump分成两个，即在Lump后面新增一个Lump，分配给所申请者 */
          jv_lump_t *lump;

          lump = (jv_lump_t *) ((u_char *) p + p->size - size);
          lump->size = size;

          lump->used = 1;
          lump->next = p->next;
          p->next = lump;

          /* prev add */
          lump->next->prev = lump;
          lump->prev = p;

          p->size -= size + sizeof(jv_lump_t);

          printf("allocate lump memory address using unfit: %lu, size:%lu\n", (jv_uint_t)(void *) (lump + 1), (jv_uint_t) size);
          return (void *) (lump + 1);
        }
      }
      p = p->next;
    } while (p != pool->pos);

    return jv_pool_alloc_block(pool, size);
  } else { /* 从 lump->size > pool->max 中查找 */
    jv_lump_t *lump;
    jv_block_t *block;

    for (block = pool->first; block != NULL; block = block->next) {
      if (block->size > size) {
        lump = (jv_lump_t *) ((u_char *) block + sizeof(jv_block_t));
        if (lump->used == 0 && (jv_uint_t)(size * 1.25 / lump->size) == 1) {
          lump->used = 1;
          return (void *) (lump + 1);
        }
      }
    }
    return jv_pool_alloc_large(pool, size);
  }
}

void *jv_pool_alloc(jv_pool_t *pool, size_t size) {
  void *v;
  v = jv_pool_slb(pool, size);
  if (v != NULL) {
    jv_memzero(v, size);
  }
  return v;
}

void *jv_pool_realloc(jv_pool_t *pool, void *ptr, size_t size) {
  if (jv_pool_free(pool, ptr) == JV_OK) {
    return jv_pool_alloc(pool, size);
  }
  return NULL;
}

static void *jv_pool_alloc_block(jv_pool_t *pool, size_t size) {
  jv_block_t *block;
  jv_lump_t *tail;
  u_char *cp;

  if (size <= 0)
    return NULL;

  cp = malloc(pool->max + sizeof(jv_block_t) + sizeof(jv_lump_t));
  if (cp == NULL) {
    printf("malloc error in jv_pool_alloc_block()\n");
    return NULL;
  }

  printf("allocate block memory address: %lu, size: %lu\n", (jv_uint_t) cp, (jv_uint_t) size);

  block = (jv_block_t *) cp;
  block->size = pool->max;
  block->next = pool->last->next;

  pool->last->next = block;
  pool->last = block;

  tail = pool->lump->prev;

  if (size > pool->max - 2 * sizeof(jv_lump_t)) { /* 新申请的block，仅生成一个lump节点,并将block的所有空闲内存分配给新节点 */
    jv_lump_t *lump;
    lump = (jv_lump_t *) (cp + sizeof(jv_block_t));
    lump->size = pool->max;
    lump->used = 1;

    lump->next = pool->lump;
    tail->next = lump;

    lump->next->prev = lump;
    lump->prev = tail;

    pool->pos = lump;
    return (void *) (lump + 1);
  } else { /* 新申请的block，生成两个lump节点，并将后一个lump节点的size个空闲空间alloc分配给申请者，前一个lump节点free的空闲空间保留
            */
    jv_lump_t *free, *alloc;
    free = (jv_lump_t *) (cp + sizeof(jv_block_t));
    free->size = pool->max - size - sizeof(jv_lump_t);
    free->used = 0;

    alloc = (jv_lump_t *) ((u_char *) free + sizeof(jv_lump_t) + free->size);
    alloc->size = size;
    alloc->used = 1;

    alloc->next = pool->lump;
    free->next = alloc;
    tail->next = free;

    alloc->next->prev = alloc;
    alloc->prev = free;
    free->prev = tail;

    pool->pos = free;
    return (void *) (alloc + 1);
  }
}

static void *jv_pool_alloc_large(jv_pool_t *pool, size_t size) {
  jv_block_t *block;
  jv_lump_t *tail, *lump;
  u_char *cp;

  cp = malloc(size + sizeof(jv_block_t) + sizeof(jv_lump_t));
  if (cp == NULL) {
    printf("malloc failed in jv_pool_alloc_large()\n");
    return NULL;
  }

  printf("allocate large memory address: %lu, size: %lu\n", (jv_uint_t) cp, size);

  block = (jv_block_t *) cp;
  block->size = size;
  block->next = pool->last->next;

  pool->last->next = block;
  pool->last = block;

  tail = pool->lump->prev;

  lump = (jv_lump_t *) (cp + sizeof(jv_block_t));
  lump->size = size;
  lump->used = 1;

  lump->next = pool->lump;
  tail->next = lump;

  lump->next->prev = lump;
  lump->prev = tail;

  pool->pos = lump;
  return (void *) (lump + 1);
}

jv_int_t jv_pool_free(jv_pool_t *pool, void *ptr) {
  jv_lump_t *prior, *idle;

  if (ptr == NULL || pool == NULL) {
    return JV_OK;
  }

  idle = (jv_lump_t *) ((u_char *) ptr - sizeof(jv_lump_t));

  if (idle->used != 1 && idle->size % JV_PLATFORM_WORD != 0) {
    printf("free's pointer is not in the memory pool\n");
    return JV_ERROR;
  }

  idle->used = 0;
  pool->pos = idle;

  printf("recycle lump memory address: %lu, size: %lu\n", (jv_uint_t)(idle + 1), (jv_uint_t) idle->size);

  if ((u_char *) idle + sizeof(jv_lump_t) + idle->size == (u_char *) idle->next) { /* 与下一个节点相临 */
    if (idle->next->used == 0) {                                                   /* 下一个节点空闲，则合并 */
      idle->size = idle->size + sizeof(jv_lump_t) + idle->next->size;
      idle->next = idle->next->next;
      idle->next->prev = idle;
      pool->pos = idle;
    }
  }

  prior = idle->prev;

  if ((u_char *) prior + sizeof(jv_lump_t) + prior->size == (u_char *) idle) { /* 与上一个节点相临 */
    if (prior->used == 0) {                                                    /* 上一个节点空闲，则合并 */
      prior->size = prior->size + sizeof(jv_lump_t) + idle->size;
      prior->next = prior->next->next;
      prior->next->prev = prior;
      pool->pos = prior;
    }
  }

  return JV_OK;
}

jv_int_t jv_pool_recycle(jv_pool_t *pool, void *ptr) {
  /* TODO */
  return JV_OK;
}

jv_int_t jv_pool_reset(jv_pool_t *pool) {
  /* TODO */
  return JV_OK;
}

void jv_pool_dump(jv_pool_t *pool, FILE *fd) {
  jv_lump_t *lump;
  jv_block_t *block;
  /* jv_uint_t i; */
  int n;

  if (pool == NULL)
    return;

  fprintf(fd, "\n┌- - - - - - - - - - - - jv memory pool monitoring - - - - - - - - - - - - -┐\n");
  fprintf(fd, "|- - - - - - - - - - - - jv_pool_lump - - - - - - - - - - - - - - - -  - - -|\n");

  /* fprintf(fd, "\n┌- - - - - - - - - - - - jv_pool_lump - - - - - - - - - - - - - - - -  - - -┐\n");*/

  lump = pool->lump;
  do {
    char buf[12];
    n = fprintf(fd, "| lump address: %lu, lump->size: %u, lump->used: %u", (unsigned long) lump, lump->size, lump->used);
    sprintf(buf, "%%%d.1s\n", 77 - n);
    fprintf(fd, buf, "|");
    lump = lump->next;
  } while (lump != pool->lump);

  /*
   for (lump = pool->lump, i = 0; i == 0 || lump != pool->lump;
   i++, lump = lump->prev) {
   fprintf(fd, "| lump address:%lu, lump->size:%u, lump->used:%u \n", (unsigned long)lump, lump->size, lump->used);
   }*/

  fprintf(fd, "|- - - - - - - - - - - - jv_pool_block - - - - - - - - - - - - - - - - - - -|\n");

  for (block = pool->first; block != NULL; block = block->next) {
    char buf[12];
    n = fprintf(fd, "| block address: %lu, block->size: %lu", (unsigned long) block, (unsigned long) block->size);
    sprintf(buf, "%%%d.1s\n", 77 - n);
    fprintf(fd, buf, "|");
  }
  fprintf(fd, "└- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - ┘\n\n");
}

void jv_pool_destroy(jv_pool_t *pool) {
  jv_block_t *block, *tmp = NULL;

  printf("destroy memory pool address: %lu\n", (jv_uint_t) pool);

  for (block = pool->first; block != NULL; block = tmp) {
    tmp = block->next;
    /* printf("destroy block:%u\n", (jv_uint_t) block); */
    free(block);
  }
  pool = NULL;
  /* printf("jv_pool_destroy block count:%d\n", i); */
}
