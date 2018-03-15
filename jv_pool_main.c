#include <assert.h>
#include <jv_pool.h>
#include <time.h>

void test1(void) {
  jv_pool_t *pool;
  void *a, *b, *c, *d, *e, *f, *g, *h, *i, *j;
  jv_uint_t base;

  jv_lump_t *lump;
  u_char *s;

  pool = jv_pool_create(128);

  base = (jv_uint_t) pool - sizeof(jv_block_t);

  printf("base: %lu, lump size: %lu\n\n", (jv_uint_t) base, (jv_uint_t) sizeof(jv_lump_t));
  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  /* jv_pool_dump(pool, stdout); */

  assert((u_char *) pool->first + sizeof(jv_block_t) == (u_char *) pool);
  assert((u_char *) pool->first + sizeof(jv_block_t) + sizeof(jv_pool_t) == (u_char *) pool->lump);
  assert((u_char *) pool + sizeof(jv_pool_t) == (u_char *) (pool->lump));

  a = jv_pool_alloc(pool, 40);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("a address: %lu, pos: %lu\n", (jv_uint_t) a - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, a) == 40);
  assert((u_char *) a == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  b = jv_pool_alloc(pool, 56);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("b address: %lu, pos: %lu\n", (jv_uint_t) b - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, b) == 56);
  assert((u_char *) b == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  c = jv_pool_alloc(pool, 32);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("c address: %lu, pos: %lu\n", (jv_uint_t) c - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, c) == 32);
  assert((u_char *) c == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  d = jv_pool_alloc(pool, 8);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("d address: %lu, pos: %lu\n", (jv_uint_t) d - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, d) == 8);
  assert((u_char *) d == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  e = jv_pool_alloc(pool, 4);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("e address: %lu, pos: %lu\n", (jv_uint_t) e - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, e) == jv_align(4, JV_WORD_SIZE / 8));
  assert((u_char *) e == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  f = jv_pool_alloc(pool, 24);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("f address: %lu, pos: %lu\n", (jv_uint_t) f - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, f) == 24);
  assert((u_char *) f == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  g = jv_pool_alloc(pool, 104);

  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n",
         (jv_uint_t) pool - base, (jv_uint_t) pool->pos - base, (jv_uint_t) pool->first - base, (jv_uint_t) pool->first->size,
         (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump - base, (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next - base);

  printf("g address: %lu, pos: %lu\n", (jv_uint_t) g - base, (jv_uint_t) pool->pos - base);

  assert(jv_pool_sizeof(pool, g) == 104);
  assert((u_char *) g == (u_char *) pool->lump->next + sizeof(jv_lump_t));

  h = jv_pool_alloc(pool, 1); /* 12 */
  assert(jv_pool_sizeof(pool, h) == jv_align(1, JV_WORD_SIZE / 8));

  i = jv_pool_alloc(pool, 310);
  assert(jv_pool_sizeof(pool, i) == jv_align(310, JV_WORD_SIZE / 8));

  j = jv_pool_alloc(pool, 12312);
  assert(jv_pool_sizeof(pool, j) == jv_align(12312, JV_WORD_SIZE / 8));

  jv_pool_dump(pool, stdout);

  jv_pool_free(pool, j);

  j = jv_pool_alloc(pool, 9000);
  assert(jv_pool_sizeof(pool, j) == jv_align(9000, JV_WORD_SIZE / 8));

  jv_pool_free(pool, h);

  jv_pool_free(pool, c);

  jv_pool_free(pool, f);

  jv_pool_free(pool, a);

  jv_pool_free(pool, e);

  jv_pool_free(pool, d);

  jv_pool_free(pool, b);

  jv_pool_free(pool, g);

  jv_pool_free(pool, i);
  jv_pool_free(pool, j);

  jv_pool_dump(pool, stdout);

  h = jv_pool_alloc(pool, 36);
  assert(jv_pool_sizeof(pool, h) == jv_align(36, JV_WORD_SIZE / 8));

  jv_pool_free(pool, h);

  s = malloc(4);
  assert(jv_pool_sizeof(pool, s) == 0);
  assert(jv_pool_free(pool, s) == JV_ERROR);
  free(s);

  lump = (jv_lump_t *) ((u_char *) pool->first + sizeof(jv_pool_t) + sizeof(jv_block_t));

  assert(lump->used == 0);
  assert(lump->size == pool->max);

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
    /* printf("allocate memory size: %lu\n", j); */
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
    /* printf("allocate memory size: %lu\n", j); */
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
  for (i = 0; i < 50000; i++) {
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

  /* printf("sizeof(jv_pool_t): %lu\n",sizeof(jv_pool_t)); */
  printf("pool: %lu, pos: %lu, block: %lu, block->size: %lu, block->next: %lu, lump: %lu, lump->size: %lu, lump->next: %lu\n", (jv_uint_t) pool,
         (jv_uint_t) pool->pos, (jv_uint_t) pool->first, (jv_uint_t) pool->first->size, (jv_uint_t) pool->first->next, (jv_uint_t) pool->lump,
         (jv_uint_t) pool->lump->size, (jv_uint_t) pool->lump->next);

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

  printf("a: %p, b: %p, c: %p, d: %p\n", a, b, c, d);

  jv_pool_destroy(pool);
}

void test7(void) {
  jv_pool_t *pool;
  unsigned i;

  pool = jv_pool_create(1024 * 16);

  srand(time(NULL));
  for (i = 0; i < 100; i++) {
    jv_uint_t j = rand() % 1001;
    jv_uint_t k = rand() % 1024 * 18;
    jv_lump_t *lump = jv_pool_alloc(pool, j * k);
    /* printf("allocate memory size: %lu\n", j); */
    if (lump == NULL) {
      printf("allocate memory error: %u\n", i);
      break;
    }
  }
  /* jv_pool_dump(pool,stdout); */
  jv_pool_destroy(pool);
}

void test8(void) {
  jv_pool_t *pool;

  pool = jv_pool_create(1024 * 16);

  jv_pool_dump(pool, stdout);

  assert(jv_pool_reset(pool)==JV_OK);

  jv_pool_alloc(pool, 4432);
  jv_pool_alloc(pool, 412);
  jv_pool_alloc(pool, 3);

  jv_pool_dump(pool, stdout);

  assert(jv_pool_reset(pool) == JV_OK);

  jv_pool_dump(pool, stdout);
  jv_pool_destroy(pool);
}

int main(int argc, char *argv[]) {
/*  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
*/
  test8();
  return 0;
}
