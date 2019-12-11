#include <stdbool.h>
#include "refmem.h"
#include <CUnit/Basic.h>
typedef struct test_struct test_t;
struct test_struct
{
  char *str;
};

void test_alloc()
{
  test_t *alloc = allocate(sizeof(test_t), NULL);
  CU_ASSERT_PTR_NOT_NULL(alloc);
}

int init_suite(void)
{
  return 0;
}
int clean_suite(void)
{
  return 0;
}

int main()
{
  CU_pSuite test_suite1 = NULL;
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(test_suite1, "Test remove", test_alloc))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
