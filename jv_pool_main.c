#include <assert.h>
#include <time.h>
#include <jv_pool.h>

void test1_for_x86(void) {
  jv_pool_t *pool;
  void *a, *b, *c, *d, *e, *f, *g, *h, *i, *j;

  jv_lump_t *lump;
  char *s;

  pool = jv_pool_create(128);

  /* printf("sizeof(jv_pool_t):%lu\n",sizeof(jv_pool_t)); */
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);

  /* jv_pool_dump(pool, stdout); */

  assert((u_char *) pool->first + sizeof(jv_block_t) == (u_char *) pool);
  assert((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t) == (u_char *) pool->lump);
  assert((u_char *) pool + sizeof(jv_pool_t) == (u_char *) (pool->lump));

  a = jv_pool_alloc(pool, 40);
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%d,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, pool->lump->size, (unsigned long) pool->lump->next);
  printf("a address:%lu,pos:%lu\n", (unsigned long) a, (unsigned long) pool->pos);
  assert((u_char *) a == (u_char *) pool + sizeof(jv_pool_t) + sizeof(jv_lump_t) + 76 + sizeof(jv_lump_t));
  /* jv_pool_dump(pool, stdout); */

  b = jv_pool_alloc(pool, 56); /* 76 */
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);
  printf("b address:%lu,pos:%lu\n", (unsigned long) b, (unsigned long) pool->pos);
  assert((u_char *) b == (u_char *) pool + sizeof(jv_pool_t) + sizeof(jv_lump_t));

  c = jv_pool_alloc(pool, 32);
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);
  printf("c address:%lu,pos:%lu\n", (unsigned long) c, (unsigned long) pool->pos);
  assert((u_char *) c == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 84 + sizeof(jv_lump_t));

  d = jv_pool_alloc(pool, 8);
  printf("pool:%lu,pos:%lu,block:%lu,block->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("d address:%lu,pos:%lu\n", (unsigned long) d, (unsigned long) pool->pos);
  assert((u_char *) d == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 64 + sizeof(jv_lump_t));

  e = jv_pool_alloc(pool, 4);
  printf("pool:%lu,pos:%lu,block:%lu,block->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("e address:%lu,pos:%lu\n", (unsigned long) e, (unsigned long) pool->pos);
  assert((u_char *) e == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 48 + sizeof(jv_lump_t));

  f = jv_pool_alloc(pool, 24);
  printf("pool:%lu,pos:%lu,block:%lu,block->next->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("f address:%lu,pos:%lu\n", (unsigned long) f, (unsigned long) pool->pos);
  assert((u_char *) f == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t));

  g = jv_pool_alloc(pool, 104);
  printf("pool:%lu,pos:%lu,block:%lu,block->next->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("g address:%lu,pos:%lu\n", (unsigned long) g, (unsigned long) pool->pos);
  assert((u_char *) g == (u_char *) pool->first->next->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 12 + sizeof(jv_lump_t));

  h = jv_pool_alloc(pool, 1); /* 12 */

  i = jv_pool_alloc(pool, 310);

  j = jv_pool_alloc(pool, 12312);

  jv_pool_free(pool, j);

  j = jv_pool_alloc(pool, 9000);

  jv_pool_free(pool, h);

  jv_pool_free(pool, c);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 84));
  assert(lump->used == 0);
  assert(lump->size == 32);

  jv_pool_free(pool, f);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t)));
  assert(lump->used == 0);
  assert(lump->size == 48);

  jv_pool_free(pool, a);
  lump = ((jv_lump_t *) ((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t) + sizeof(jv_lump_t) + 76));
  assert(lump->used == 0);
  assert(lump->size == 40);

  jv_pool_free(pool, e);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 48));
  assert(lump->used == 0);
  assert(lump->size == 4);

  jv_pool_free(pool, d);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 64));
  assert(lump->used == 0);
  assert(lump->size == 52);

  jv_pool_free(pool, b);
  lump = ((jv_lump_t *) ((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t)));
  assert(lump->used == 0);
  assert(lump->size == 128);

  jv_pool_free(pool, g);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 12));
  assert(lump->used == 0);
  assert(lump->size == 104);

  jv_pool_free(pool, i);
  jv_pool_free(pool, j);

  jv_pool_dump(pool, stdout);

  h = jv_pool_alloc(pool, 36);

  jv_pool_free(pool, h);

  s = malloc(4);
  assert(jv_pool_free(pool, s) == JV_ERROR);
  free(s);

  jv_pool_destroy(pool);
}

void test1_for_x64(void) {
  jv_pool_t *pool;
  void *a, *b, *c, *d, *e, *f, *g, *h, *i, *j;

  jv_lump_t *lump;
  char *s;

  pool = jv_pool_create(128);

  /* printf("sizeof(jv_pool_t):%lu\n",sizeof(jv_pool_t)); */
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);

  /* jv_pool_dump(pool, stdout); */

  assert((u_char *) pool->first + sizeof(jv_block_t) == (u_char *) pool);
  assert((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t) == (u_char *) pool->lump);
  assert((u_char *) pool + sizeof(jv_pool_t) == (u_char *) (pool->lump));

  a = jv_pool_alloc(pool, 40);
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);
  printf("a address:%lu,pos:%lu\n", (unsigned long) a, (unsigned long) pool->pos);
  assert((u_char *) a == (u_char *) pool + sizeof(jv_pool_t) + sizeof(jv_lump_t) + 76 + sizeof(jv_lump_t));
  /* jv_pool_dump(pool, stdout); */

  b = jv_pool_alloc(pool, 56); /* 76 */
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);
  printf("b address:%lu,pos:%lu\n", (unsigned long) b, (unsigned long) pool->pos);
  assert((u_char *) b == (u_char *) pool + sizeof(jv_pool_t) + sizeof(jv_lump_t));

  c = jv_pool_alloc(pool, 32);
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);
  printf("c address:%lu,pos:%lu\n", (unsigned long) c, (unsigned long) pool->pos);
  assert((u_char *) c == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 84 + sizeof(jv_lump_t));

  d = jv_pool_alloc(pool, 8);
  printf("pool:%lu,pos:%lu,block:%lu,block->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("d address:%lu,pos:%lu\n", (unsigned long) d, (unsigned long) pool->pos);
  assert((u_char *) d == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 64 + sizeof(jv_lump_t));

  e = jv_pool_alloc(pool, 4);
  printf("pool:%lu,pos:%lu,block:%lu,block->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("e address:%lu,pos:%lu\n", (unsigned long) e, (unsigned long) pool->pos);
  assert((u_char *) e == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 48 + sizeof(jv_lump_t));

  f = jv_pool_alloc(pool, 24);
  printf("pool:%lu,pos:%lu,block:%lu,block->next->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("f address:%lu,pos:%lu\n", (unsigned long) f, (unsigned long) pool->pos);
  assert((u_char *) f == (u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t));

  g = jv_pool_alloc(pool, 104);
  printf("pool:%lu,pos:%lu,block:%lu,block->next->next:%lu,lump:%lu,lump->next:%lu\n", (unsigned long) pool, (unsigned long) pool->pos,
         (unsigned long) pool->first, (unsigned long) pool->first->next->next, (unsigned long) pool->lump, (unsigned long) pool->lump->next);
  printf("g address:%lu,pos:%lu\n", (unsigned long) g, (unsigned long) pool->pos);
  assert((u_char *) g == (u_char *) pool->first->next->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 12 + sizeof(jv_lump_t));

  h = jv_pool_alloc(pool, 1); /* 12 */

  i = jv_pool_alloc(pool, 310);

  j = jv_pool_alloc(pool, 12312);

  jv_pool_free(pool, j);

  j = jv_pool_alloc(pool, 9000);

  jv_pool_free(pool, h);

  jv_pool_free(pool, c);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 84));
  assert(lump->used == 0);
  assert(lump->size == 32);

  jv_pool_free(pool, f);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t)));
  assert(lump->used == 0);
  assert(lump->size == 48);

  jv_pool_free(pool, a);
  lump = ((jv_lump_t *) ((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t) + sizeof(jv_lump_t) + 76));
  assert(lump->used == 0);
  assert(lump->size == 40);

  jv_pool_free(pool, e);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 48));
  assert(lump->used == 0);
  assert(lump->size == 4);

  jv_pool_free(pool, d);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 64));
  assert(lump->used == 0);
  assert(lump->size == 52);

  jv_pool_free(pool, b);
  lump = ((jv_lump_t *) ((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t)));
  assert(lump->used == 0);
  assert(lump->size == 128);

  jv_pool_free(pool, g);
  lump = ((jv_lump_t *) ((u_char *) pool->first->next->next + sizeof(jv_block_t) + sizeof(jv_lump_t) + 12));
  assert(lump->used == 0);
  assert(lump->size == 104);

  jv_pool_free(pool, i);
  jv_pool_free(pool, j);

  jv_pool_dump(pool, stdout);

  h = jv_pool_alloc(pool, 36);

  jv_pool_free(pool, h);

  s = malloc(4);
  assert(jv_pool_free(pool, s) == JV_ERROR);
  free(s);

  jv_pool_destroy(pool);
}

void test2(void) {
  jv_pool_t *pool;
  unsigned i;

  pool = jv_pool_create(1024 * 16);

  srand(time(NULL));
  for (i = 0; i < 10000; i++) {
    jv_uint_t j = rand() % 1001;
    jv_uint_t k = rand() % 10001;
    jv_lump_t *lump = jv_pool_alloc(pool, j * k);
    /* printf("allocate memory size:%lu\n", j); */
    if (lump == NULL) {
      printf("allocate memory error: %u\n", i);
      break;
    }
    assert(jv_pool_free(pool, lump) == JV_OK);
  }
  /* jv_pool_dump(pool,stdout); */
  jv_pool_destroy(pool);
}

void test3(void) {
  jv_pool_t *pool;
  unsigned i;

  pool = jv_pool_create(1024 * 16);

  srand(time(NULL));
  for (i = 0; i < 10000; i++) {
    jv_uint_t j = rand() % 1001;
    jv_lump_t *lump = jv_pool_alloc(pool, j);
    /* printf("allocate memory size:%lu\n", j); */
    if (lump == NULL) {
      printf("allocate memory error: %u\n", i);
      break;
    }
    assert(jv_pool_free(pool, lump) == JV_OK);
  }
  /* jv_pool_dump(pool,stdout); */
  jv_pool_destroy(pool);
}

void test4(void) {
  jv_pool_t *pool;
  unsigned i, *s;
  pool = jv_pool_create(1024 * 16);

  srand(time(NULL));
  for (i = 0; i < 10000; i++) {
    jv_uint_t j = rand() % 1001;
    s = malloc(j);
    assert(jv_pool_free(pool, s) == JV_ERROR);
    free(s);
  }
  jv_pool_destroy(pool);
}

void test5(void) {
  jv_pool_t *pool;

  pool = jv_pool_create(1024 * 16);

  jv_pool_alloc(pool, 432141);
  jv_pool_alloc(pool, 10000);
  jv_pool_alloc(pool, 10000);

  assert(jv_pool_free(pool, jv_pool_alloc(pool, 432141)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 4)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 634)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 10000)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 10000)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 324542)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 0x1fffffff)) == JV_OK);
  assert(jv_pool_free(pool, jv_pool_alloc(pool, 41233)) == JV_OK);

  /* jv_pool_alloc(pool, 0x1fffffff); */

  /* jv_pool_dump(pool,stdout); */
  jv_pool_destroy(pool);
}

void test6(void) {
  jv_pool_t *pool;
  char *a, *b, *c, *d;

  pool = jv_pool_create(128);

  /* printf("sizeof(jv_pool_t):%lu\n",sizeof(jv_pool_t)); */
  printf("pool:%lu,pos:%lu,block:%lu,block->size:%ld,block->next:%lu,lump:%lu,lump->size:%ld,lump->next:%lu\n", (unsigned long) pool,
         (unsigned long) pool->pos, (unsigned long) pool->first, (unsigned long) pool->first->size, (unsigned long) pool->first->next,
         (unsigned long) pool->lump, (unsigned long) pool->lump->size, (unsigned long) pool->lump->next);

  jv_pool_dump(pool, stdout);

  assert((u_char *) pool->first + sizeof(jv_block_t) == (u_char *) pool);
  assert((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t) == (u_char *) pool->lump);
  assert((u_char *) pool + sizeof(jv_pool_t) == (u_char *) (pool->lump));

  a = jv_pool_alloc(pool, 40);

  b = jv_pool_alloc(pool, 56);

  c = jv_pool_alloc(pool, 32);

  jv_pool_dump(pool, stdout);

  d = jv_pool_realloc(pool, c, 48);

  assert(c - 16 == d);

  jv_pool_dump(pool, stdout);

  printf("%s, %s, %s, %s\n", a, b, c, d);

  jv_pool_destroy(pool);
}

int main(int argc, char *argv[]) {
  test1_for_x86();

  
  /*  test2();
    test3();
    test5();
    test6(); */
   

 /*  test4();  */

  return 0;
}
