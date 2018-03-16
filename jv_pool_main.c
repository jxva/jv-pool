#include <assert.h>
#include <jv_pool.h>

int main(int argc, char *argv[]) {
  jv_pool_t *pool;
  u_char *a;

  pool = jv_pool_create(JV_ALLOC_DEFAULT_SIZE);

  a = jv_pool_alloc(pool, 32);

  jv_memset(a, 'a', 32);

  a[31] = '\0';

  printf("a:%s, len:%lu\n", a, jv_pool_sizeof(pool, a));

  a= jv_pool_realloc(pool, a, 64);

  assert(jv_pool_sizeof(pool, a) == 64);

  jv_memset(a, '1', 64);

  a[63] = '\0';

  printf("a:%s, len:%lu\n", a, jv_pool_sizeof(pool, a));

  assert(jv_pool_recycle(pool, a) == JV_OK);

  assert(jv_pool_free(pool, a) == JV_OK);

  jv_pool_dump(pool, stdout);

  jv_pool_destroy(pool);

  return 0;
}
