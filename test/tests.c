#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <CUnit/Basic.h>




void test_alloc()
{
  test_t *alloc = allocate(sizeof(test_t), NULL);
  CU_ASSERT_PTR_NOT_NULL(alloc);
  deallocate(alloc);
}

void test_alloc_array()
{
  test_t *alloc = allocate_array(10, sizeof(test_t), NULL);
  alloc->str = "test";
  CU_ASSERT_PTR_NOT_NULL(alloc);
  deallocate(alloc);
}


void test_destructor_null()
{
  test_t *alloc = allocate_array(10, sizeof(test_t), NULL);
  deallocate(alloc);
 
}

void test_destruct_string()
{
  test_t *alloc = allocate(sizeof(test_t), destructor_string);
  alloc->str = strdup("test");
  deallocate(alloc);
}

void test_cascade_limit()
{
  int expected = 100;
  set_cascade_limit(100);
  size_t limit = get_cascade_limit();
  CU_ASSERT_EQUAL(limit, expected);
  set_cascade_limit(40);
  limit = get_cascade_limit();
  CU_ASSERT_NOT_EQUAL(limit, expected);
  set_cascade_limit(-20);
  limit = get_cascade_limit();
  expected = 40;
  CU_ASSERT_EQUAL(limit, expected);
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
      (NULL == CU_add_test(test_suite1, "alloc", test_alloc)) ||
      (NULL == CU_add_test(test_suite1, "alloc array", test_alloc_array))||
      (NULL == CU_add_test(test_suite1, "destruct string", test_destruct_string)) ||
      (NULL == CU_add_test(test_suite1, "destructor null", test_destructor_null)) ||
      (NULL == CU_add_test(test_suite1, "cascade limit", test_cascade_limit))
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
