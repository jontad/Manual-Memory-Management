#include <stdbool.h>
#include "../src/refmem.h"
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

void test_alloc_array()
{
  test_t *alloc = allocate_array(10, sizeof(test_t), NULL);
  alloc->str = "test";
  CU_ASSERT_PTR_NOT_NULL(alloc);
}

void destructor_string(obj *object)
{
  free(object);
}

void test_destructor_null()
{
  test_t *alloc = allocate_array(10, sizeof(test_t), NULL);
  //retain(alloc);
  deallocate(alloc);
  CU_ASSERT_PTR_NULL(alloc);
}

void test_destruct_string()
{
  test_t *alloc = allocate(sizeof(test_t), destructor_string);
  //retain(alloc); Change after retain exists
  alloc->str = strdup("hej");
  deallocate(alloc);
  CU_ASSERT_PTR_NULL(alloc);
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
      (NULL == CU_add_test(test_suite1, "destructor null", test_destructor_null))
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