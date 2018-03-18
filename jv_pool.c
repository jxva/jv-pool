#include <jv_pool.h>

static void *jv_pool_slb(jv_pool_t *pool, size_t size);

static void *jv_pool_alloc_block(jv_pool_t *pool, size_t size);

static void *jv_pool_alloc_huge(jv_pool_t *pool, size_t size);

jv_pool_t *jv_pool_create(size_t size, unsigned mode) {
  jv_pool_t *pool;
  jv_block_t *block;
  jv_lump_t *lump;
  u_char *cp;

  if (size < JV_POOL_MIN_SIZE) {
    size = JV_POOL_DEFAULT_SIZE;
  }

  if (size > JV_POOL_MAX_SIZE) {
    printf("jv_pool_create() failed, allow max memory size is %u\n", JV_POOL_MAX_SIZE);
    return (jv_pool_t *) NULL;
  }

  size = jv_align(size, JV_WORD_SIZE / 8);

  cp = malloc(size + sizeof(jv_block_t) + sizeof(jv_pool_t) + sizeof(jv_lump_t));
  if (cp == NULL) {
    printf("jv_pool_create() failed, alloc size is %lu\n", size);
    return (jv_pool_t *) NULL;
  }

  block = (jv_block_t *) cp;
  pool = (jv_pool_t *) (cp + sizeof(jv_block_t));
  lump = (jv_lump_t *) (cp + sizeof(jv_block_t) + sizeof(jv_pool_t));

  block->size = size;
  block->next = NULL;

  pool->size = size;
  pool->first = pool->last = block;
  pool->idle = pool->lump = lump->next = lump->prev = lump;
  pool->block_count = 1;
  pool->lump_count = 1;

  lump->size = size;
  lump->used = 0;

  pool->mode = mode == JV_POOL_QUICK_MODE ? JV_POOL_QUICK_MODE : JV_POOL_SAFE_MODE;

  printf("create a new memory pool, size is %lu\n", (jv_uint_t) size);

  return pool;
}

static void *jv_pool_slb(jv_pool_t *pool, size_t size) {
  if (size <= pool->size) {
    jv_lump_t *p;

    p = pool->idle;
    do {
      if (p->used == 0 && p->size <= pool->size && p->size >= size) { /* first fit */
        if (p->size <= size + 2 * sizeof(jv_lump_t)) {                /* fit  */
          p->used = 1;
          printf("alloc memory in lump using fit, size is: %u\n", p->size);
          return (void *) (p + 1);
        } else { /* best fit */
          jv_lump_t *lump;

          lump = (jv_lump_t *) ((u_char *) p + p->size - size);
          lump->size = size;

          lump->used = 1;
          lump->next = p->next;
          p->next = lump;

          lump->next->prev = lump;
          lump->prev = p;

          p->size -= size + sizeof(jv_lump_t);

          pool->lump_count++;

          printf("alloc memory in lump using best fit, size is: %u\n", lump->size);
          return (void *) (lump + 1);
        }
      }
      p = p->next;
    } while (p != pool->idle);

    return jv_pool_alloc_block(pool, size);
  } else { /* unfit in lump pool */
    jv_lump_t *lump;
    jv_block_t *block;

    for (block = pool->first; block != NULL; block = block->next) {
      if (block->size >= size) {
        lump = (jv_lump_t *) ((u_char *) block + sizeof(jv_block_t));
        if (lump->used == 0 && (jv_uint_t)(size * 1.25 / lump->size) == 1) { /* alloc block fit */
          lump->size = block->size;
          lump->used = 1;
          printf("alloc memory in block using fit, size is: %u\n", lump->size);
          return (void *) (lump + 1);
        }
      }
    }

    return jv_pool_alloc_huge(pool, size);
  }
}

void *jv_pool_alloc(jv_pool_t *pool, size_t size) {
  void *v;

  if (size <= 0) {
    printf("alloc memory must be greater than zero\n");
    return NULL;
  }

  if (size > JV_POOL_MAX_SIZE) {
    printf("alloc memory is too huge, allow max memory size is: %u\n", JV_POOL_MAX_SIZE);
    return NULL;
  }

  size = jv_align(size, JV_WORD_SIZE / 8);

  v = jv_pool_slb(pool, size);
  if (v != NULL) {
    jv_memzero(v, size);
  }
  return v;
}

size_t jv_pool_sizeof(jv_pool_t *pool, void *ptr) {
  jv_lump_t *lump;

  if (jv_pool_exist(pool, ptr) == JV_ERROR) {
    return 0;
  }

  lump = (jv_lump_t *) ((u_char *) ptr - sizeof(jv_lump_t));

  if (lump->size % (JV_WORD_SIZE / 8) != 0) {
    return 0;
  }

  return lump->size;
}

jv_int_t jv_pool_exist(jv_pool_t *pool, void *ptr) {
  jv_lump_t *lump;
  size_t l;

  if (pool == NULL || ptr == NULL) {
    return JV_ERROR;
  }

  l = sizeof(jv_lump_t);

  if (pool->mode == JV_POOL_QUICK_MODE) {
    lump = (jv_lump_t *) ((u_char *) ptr - l);

    if (lump->size % (JV_WORD_SIZE / 8) != 0) {
      printf("ptr not exist in memroy pool, %u\n", lump->size);
      return JV_ERROR;
    }

    return JV_OK;
  } else {
    lump = pool->lump;

    do {
      if ((u_char *) lump + l == (u_char *) ptr /*&& lump->size % (JV_WORD_SIZE / 8) != 0*/) {
        return JV_OK;
      }
      lump = lump->next;
    } while (lump != pool->lump);

    printf("ptr not exist in memroy pool\n");
    return JV_ERROR;
  }
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

  cp = malloc(pool->size + sizeof(jv_block_t) + sizeof(jv_lump_t));
  if (cp == NULL) {
    printf("alloc block memory failed, alloc size is %lu\n", (jv_uint_t) pool->size);
    return NULL;
  }

  block = (jv_block_t *) cp;
  block->size = pool->size;
  block->next = pool->last->next;

  pool->last->next = block;
  pool->last = block;

  pool->block_count++;
  pool->lump_count++;

  tail = pool->lump->prev;

  if (size > pool->size - 2 * sizeof(jv_lump_t)) {
    jv_lump_t *lump;
    lump = (jv_lump_t *) (cp + sizeof(jv_block_t));
    lump->size = pool->size;
    lump->used = 1;

    lump->next = pool->lump;
    tail->next = lump;

    lump->next->prev = lump;
    lump->prev = tail;

    pool->idle = lump;

    printf("alloc a new block with only one lump, alloc all free memory of ths block to applicant\n");

    return (void *) (lump + 1);
  } else {
    jv_lump_t *free, *alloc;
    free = (jv_lump_t *) (cp + sizeof(jv_block_t));
    free->size = pool->size - size - sizeof(jv_lump_t);
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

    pool->idle = free;
    pool->lump_count++;

    printf("alloc a new block with two lumps, first lump reserved for new block, last one lump alloc to applicant\n");
    return (void *) (alloc + 1);
  }
}

static void *jv_pool_alloc_huge(jv_pool_t *pool, size_t size) {
  jv_block_t *block;
  jv_lump_t *tail, *lump;
  u_char *cp;

  cp = malloc(size + sizeof(jv_block_t) + sizeof(jv_lump_t));
  if (cp == NULL) {
    printf("alloc huge memory failed, alloc size is %lu\n", (jv_uint_t) size);
    return NULL;
  }

  block = (jv_block_t *) cp;
  block->size = size;
  block->next = pool->last->next;

  pool->last->next = block;
  pool->last = block;
  pool->block_count++;
  pool->lump_count++;

  tail = pool->lump->prev;

  lump = (jv_lump_t *) (cp + sizeof(jv_block_t));
  lump->size = size;
  lump->used = 1;

  lump->next = pool->lump;
  tail->next = lump;

  lump->next->prev = lump;
  lump->prev = tail;

  pool->idle = lump;

  printf("alloc a new huge block with only one lump, alloc all free memory of ths huge block to applicant\n");
  return (void *) (lump + 1);
}

jv_int_t jv_pool_free(jv_pool_t *pool, void *ptr) {
  jv_lump_t *prior, *idle;

  if (jv_pool_exist(pool, ptr) == JV_ERROR) {
    return JV_ERROR;
  }

  idle = (jv_lump_t *) ((u_char *) ptr - sizeof(jv_lump_t));

  if (/*idle->used != 1 &&*/ idle->size % (JV_WORD_SIZE / 8) != 0) {
    printf("free's pointer is not in the memory pool\n");
    return JV_ERROR;
  }

  idle->used = 0;
  pool->idle = idle;

  if ((u_char *) idle + sizeof(jv_lump_t) + idle->size == (u_char *) idle->next) { /* nearby the next lump */
    if (idle->next->used == 0) {                                                   /* next lump is free，then merge */
      idle->size = idle->size + sizeof(jv_lump_t) + idle->next->size;
      idle->next = idle->next->next;
      idle->next->prev = idle;
      pool->idle = idle;
      pool->lump_count--;
    }
  }

  prior = idle->prev;

  if ((u_char *) prior + sizeof(jv_lump_t) + prior->size == (u_char *) idle) { /* nearby the previous lump*/
    if (prior->used == 0) {                                                    /*  previous lump is free，then merge */
      prior->size = prior->size + sizeof(jv_lump_t) + idle->size;
      prior->next = prior->next->next;
      prior->next->prev = prior;
      pool->idle = prior;
      pool->lump_count--;
    }
  }

  return JV_OK;
}

jv_int_t jv_pool_recycle(jv_pool_t *pool, void *ptr) {
  jv_lump_t *lump;

  if (jv_pool_exist(pool, ptr) == JV_ERROR) {
    return JV_ERROR;
  }

  lump = (jv_lump_t *) ((u_char *) ptr - sizeof(jv_lump_t));

  if (lump->size % (JV_WORD_SIZE / 8) != 0) {
    return JV_ERROR;
  }

  lump->used = 0;

  return JV_OK;
}

jv_int_t jv_pool_reset(jv_pool_t *pool) {
  jv_block_t *first, *block, *tmp = NULL;
  jv_lump_t *lump;

  first = pool->first;

  if (pool == NULL) {
    return JV_ERROR;
  }

  lump = pool->lump;
  lump->size = pool->size;
  lump->used = 0;
  lump->next = lump->prev = lump;

  for (block = first->next; block != NULL; block = tmp) {
    tmp = block->next;
    free(block);
  }

  block = first;
  block->size = pool->size;
  block->next = NULL;

  pool->first = pool->last = block;
  pool->idle = lump;
  pool->block_count = 1;
  pool->lump_count = 1;

  return JV_OK;
}

void jv_pool_destroy(jv_pool_t *pool) {
  jv_block_t *block, *tmp = NULL;

  printf("destory a memory pool, size is %lu\n", (jv_uint_t) pool->size);

  for (block = pool->first; block != NULL; block = tmp) {
    tmp = block->next;
    free(block);
  }
  pool = NULL;
}

void jv_pool_dump(jv_pool_t *pool, FILE *fd) {
  jv_lump_t *lump;
  jv_block_t *block;
  /* jv_uint_t i; */

  if (pool == NULL) {
    return;
  }

  fprintf(fd, "\n[ pool monitor, block count: %u, lump count: %u ]\n", pool->block_count, pool->lump_count);
  fprintf(fd, "lumps: \n");

  lump = pool->lump;
  do {
    fprintf(fd, "\taddress: %-12lu size: %-10lu used: %lu\n", (unsigned long) lump, (jv_uint_t) lump->size, (jv_uint_t) lump->used);
    lump = lump->next;
  } while (lump != pool->lump);

  /*
   for (lump = pool->lump, i = 0; i == 0 || lump != pool->lump;
   i++, lump = lump->prev) {
   fprintf(fd, "| lump address:%lu, lump->size:%u, lump->used:%u \n", (unsigned long)lump, lump->size, lump->used);
   }*/

  fprintf(fd, "blocks\n");

  for (block = pool->first; block != NULL; block = block->next) {
    fprintf(fd, "\taddress: %-12lu size: %-10lu\n", (unsigned long) block, (unsigned long) block->size);
  }
}
