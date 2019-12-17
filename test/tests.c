#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <CUnit/Basic.h>

void test_alloc()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  CU_ASSERT_PTR_NOT_NULL(alloc);
  deallocate(alloc);
}

void test_alloc_array()
{
  string_t *alloc = allocate_array(10, sizeof(string_t), NULL);
  alloc->str = "test";
  CU_ASSERT_PTR_NOT_NULL(alloc);
  deallocate(alloc);
}


void test_destructor_null()
{
  string_t *alloc = allocate_array(10, sizeof(string_t), NULL);
  deallocate(alloc);
 
}

void test_destruct_string()
{
  string_t *alloc = allocate(sizeof(string_t), destructor_string);
  alloc->str = strdup("test");
  deallocate(alloc);
}

void test_retain()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  retain(alloc);
  CU_ASSERT_EQUAL(1,rc(alloc));
  release(alloc);
}

void test_retain_null()
{
  string_t *alloc = NULL;
  retain(alloc);
  CU_ASSERT_PTR_NULL(alloc);
}


void test_release_null()
{
  string_t *alloc = NULL;
  release(alloc);
  CU_ASSERT_PTR_NULL(alloc);
}


void test_rc()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  retain(alloc);
  retain(alloc);
  CU_ASSERT_EQUAL(2,rc(alloc));
  release(alloc);
  CU_ASSERT_EQUAL(1,rc(alloc));
  release(alloc);
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

void test_destructor_linked_list()
{
  list_t *list = linked_list_create();
  char *str1 = strdup("hello");
  char *str2 = strdup("world");
  linked_list_append(list, str1);
  linked_list_append(list, str2);
  size_t actual_size = linked_list_size(list);
  CU_ASSERT_EQUAL(actual_size, 2);
  deallocate(list);
}

void test_destructor_linked_list2()
{
  list_t *list = linked_list_create();
  
  for (int i = 0; i < 1000; i++)
    {
      char *str = strdup("hello");
      linked_list_append(list, str);
    }

  size_t actual_size = linked_list_size(list);
  CU_ASSERT_EQUAL(actual_size, 1000);

  set_cascade_limit(100);
  deallocate(list);
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
      /*(NULL == CU_add_test(test_suite1, "alloc", test_alloc)) ||
      (NULL == CU_add_test(test_suite1, "alloc array", test_alloc_array))||
      (NULL == CU_add_test(test_suite1, "destruct string", test_destruct_string)) ||
      (NULL == CU_add_test(test_suite1, "destructor null", test_destructor_null)) ||
      (NULL == CU_add_test(test_suite1, "retain", test_retain))||
      (NULL == CU_add_test(test_suite1, "retain null", test_retain_null))|
      (NULL == CU_add_test(test_suite1, "release null", test_release_null))||*/
      //(NULL == CU_add_test(test_suite1, "rc", test_rc))||
      //(NULL == CU_add_test(test_suite1, "cascade_limit", test_cascade_limit)) ||
      (NULL == CU_add_test(test_suite1, "linked_list", test_destructor_linked_list)) //||
      //(NULL == CU_add_test(test_suite1, "linked_list2", test_destructor_linked_list2))
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
