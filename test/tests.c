#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <CUnit/Basic.h>
#include "../src/linked_list.h"




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


void test_alloc_array_loop()
{
  char **alloc = allocate_array(10, sizeof(char *), destructor_string_array);
  for(int i = 0; i < 10; i++)
    {
      alloc[i] = strdup("test");
    }
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
/*
void test_destructor_linked_list()
{
  list_t *list = allocate(sizeof(list_t), destructor_linked_list);
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  
  char *str = strdup("hello");
  char *string = strdup("world");
  linked_list_append(list, str);
  linked_list_append(list, string);
  size_t actual_size = linked_list_size(list);
  CU_ASSERT_EQUAL(actual_size, 2);
  deallocate(list);
}
*/
void test_shutdown()
{
  create_list();
  shutdown();
}

void test_cleanup()
{
  ioopm_list_t *list = create_list(); 
  string_t *str = allocate(sizeof(string_t),NULL);
  string_t *string = allocate(sizeof(string_t),NULL);
  str->str = "Hello";
  string->str = "World";
  size_t actual_size = ioopm_linked_list_size(list);
  CU_ASSERT_EQUAL(actual_size, 2);
  cleanup();
  actual_size = ioopm_linked_list_size(list);
  CU_ASSERT_EQUAL(actual_size, 0);
  shutdown();
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
      (NULL == CU_add_test(test_suite1, "retain", test_retain))||
      (NULL == CU_add_test(test_suite1, "retain null", test_retain_null))||
      (NULL == CU_add_test(test_suite1, "alloc array loop", test_alloc_array_loop))||
      (NULL == CU_add_test(test_suite1, "release null", test_release_null))||
      (NULL == CU_add_test(test_suite1, "rc", test_rc))||
      (NULL == CU_add_test(test_suite1, "cascade_limit", test_cascade_limit)) ||
      /*(NULL == CU_add_test(test_suite1, "linked_list", test_destructor_linked_list)) ||*/
      (NULL == CU_add_test(test_suite1, "cleanup", test_cleanup))
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
