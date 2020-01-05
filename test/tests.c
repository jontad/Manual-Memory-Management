#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <CUnit/Basic.h>
#include "../src/linked_list.h"


void test_alloc()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  alloc->str = NULL;
  CU_ASSERT_PTR_NOT_NULL(alloc);
  release(alloc);
}

void test_alloc_zero_bytes()
{
  obj *alloc = allocate(0, NULL);
  deallocate(alloc);
  shutdown();
}

void test_alloc_array()
{
  string_t *alloc = allocate_array(10, sizeof(string_t), NULL);
  alloc->str = "test";
  CU_ASSERT_PTR_NOT_NULL(alloc);
  release(alloc);
}
void test_alloc_array_zero_bytes()
{
  obj *alloc = allocate_array(10,0, NULL);
  deallocate(alloc);
  shutdown();
}

void test_alloc_array_loop()
{
  char **alloc = allocate_array(10, sizeof(char *), destructor_string_array);
  for(int i = 0; i < 10; i++)
    {
      alloc[i] = strdup("test");
    }
  CU_ASSERT_PTR_NOT_NULL(alloc);
  release(alloc);
}

void test_destructor_null()
{
  string_t *alloc = allocate_array(10, sizeof(string_t), NULL);
  alloc->str = NULL;
  release(alloc);
 
}

void test_destruct_default()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  alloc->str = allocate(sizeof(char *), NULL);
  release(alloc);

  shutdown();
}

void test_alloc_array_struct()
{
  string_t **alloc = allocate_array(10, sizeof(string_t *), NULL);
  for(int i = 0; i < 10; i++)
    {
      alloc[i] = allocate(sizeof(string_t),NULL);
      alloc[i]->str = allocate(sizeof(char *),NULL);
    }
  deallocate(alloc);
  shutdown();
}

void test_destruct_default_array()
{
  char **alloc = allocate_array(5,sizeof(char *),NULL);
  for(int i = 0; i < 5; i++)
    {
      alloc[i] = allocate(sizeof(char *), NULL);
    }
  deallocate(alloc);
  shutdown();
}

void test_destruct_default_several_ptrs()
{
  ptr_t *alloc = allocate(sizeof(ptr_t), NULL);
  
  alloc->str_struct = allocate(sizeof(string_t),NULL);
  alloc->int_struct = allocate(sizeof(ourInt_t),NULL);
  alloc->str = allocate(sizeof(char *), NULL);
  
  retain(alloc->str_struct);
  retain(alloc->int_struct);
  retain(alloc->str);

  release(alloc);  
  shutdown();
}

void test_destruct_string()
{
  string_t *alloc = allocate(sizeof(string_t), destructor_string);
  alloc->str = strdup("test");
  release(alloc);
}

void test_retain()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  alloc->str = NULL;
  retain(alloc);
  CU_ASSERT_EQUAL(1,rc(alloc));
  release(alloc);
}


void test_retain_overflow()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  alloc->str = NULL;
  for(int i = 0; i < 300; i++) retain(alloc);
  CU_ASSERT_EQUAL(255,rc(alloc));
  shutdown();
}


void test_release_underflow()
{
  string_t *alloc = allocate(sizeof(string_t), NULL);
  alloc->str = NULL;
  release(alloc);
  shutdown();
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
  alloc->str = NULL;
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

void test_shutdown()
{
  create_list();
  shutdown();
}

void test_cleanup()
{
  ioopm_list_t *pointer_list = linked_list_get();
  string_t *str = allocate(sizeof(string_t),NULL);
  string_t *string = allocate(sizeof(string_t),NULL);
  str->str = "Hello";
  string->str = "World";
  size_t actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 2);
  cleanup();
  actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 0);
  shutdown();
}


void test_cleanup_dif_destructors()
{
  char **alloc = allocate_array(10, sizeof(char *), destructor_string_array);
  for(int i = 0; i < 10; i++)
    {
      alloc[i] = strdup("test");
    }
  string_t *alloc_str = allocate(sizeof(string_t),destructor_string);
  alloc_str->str = strdup("Test");
  cleanup();
  shutdown();
}



void test_cleanup_empty()
{
  ioopm_list_t *pointer_list = linked_list_get();
  cleanup();
  CU_ASSERT_EQUAL(ioopm_linked_list_size(pointer_list),0);
  shutdown();
}


void test_cleanup_and_deallocate()
{
  string_t *str1 = allocate(sizeof(string_t),NULL);
  string_t *str2 = allocate(sizeof(string_t),NULL);
  str1->str = "Hello";
  str2->str = "World";

  ioopm_list_t *pointer_list = linked_list_get();
  
  size_t actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 2);

  deallocate(str1);
  
  actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 1);

  cleanup();

  actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 0);
  
  shutdown();
}

void test_cleanup_retain()
{
  string_t *str1 = allocate(sizeof(string_t),NULL);
  string_t *str2 = allocate(sizeof(string_t),NULL);
  str1->str = "Hello";
  str2->str = "World";

  ioopm_list_t *pointer_list = linked_list_get();
  
  size_t actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 2);

  retain(str1);
  
  cleanup();

  actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 1);
  
  shutdown();
}

void test_shutdown_with_allocs()
{
  string_t *str1 = allocate(sizeof(string_t),NULL);
  string_t *str2 = allocate(sizeof(string_t),NULL);
  string_t *str3 = allocate(sizeof(string_t),NULL);
  string_t *str4 = allocate(sizeof(string_t),NULL);
  str1->str = NULL;
  str2->str = NULL;
  str3->str = NULL;
  str4->str = NULL;
  ioopm_list_t *pointer_list = linked_list_get();
  size_t actual_size = ioopm_linked_list_size(pointer_list);
  CU_ASSERT_EQUAL(actual_size, 4);
  
  shutdown();
}

void test_allocate_dif_structs()
{
  string_t *str1 = allocate(sizeof(string_t),NULL);
  string_t *str2 = allocate(sizeof(string_t),NULL);
  ourInt_t *int1 = allocate(sizeof(ourInt_t),NULL);
  ourInt_t *int2 = allocate(sizeof(ourInt_t),NULL);
  str1->str = NULL;
  str2->str = NULL;
  int1->k = 0;
  int2->k = 0;

  cleanup();
  shutdown();
}

void test_cascade_free()
{
  size_reset();
  ioopm_list_t *pointer_list = linked_list_get();
  CU_ASSERT_EQUAL(ioopm_linked_list_size(pointer_list), 0);
  
  list_t *list = list_create(); //Skapar en vanlig lista 
  retain(list);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(pointer_list), 1);
  set_cascade_limit(100);
  for(int i = 0; i < 200; ++i)
    {
      linked_list_append(); //Skapar bara en ny link och placerar den sist i listan (vars element är null)
    }
  CU_ASSERT_EQUAL(200, linked_list_size());
  CU_ASSERT_EQUAL(201, ioopm_linked_list_size(pointer_list)); // The list + all 200 links
  release(list);
  CU_ASSERT_EQUAL(101, ioopm_linked_list_size(pointer_list)); // Should have deallocated 100 objects (OBS: INCLUDING THE LIST)
  CU_ASSERT_EQUAL(101, linked_list_size()); // Has 101 links left, since only 99 LINKS were released
  shutdown();
}

void test_cascade_free_alloc() //Test with using alloc after cascade limit is reached
{
  size_reset();
  list_t *list = list_create();
  retain(list);
  set_cascade_limit(100);
  CU_ASSERT_EQUAL(1, ioopm_linked_list_size(linked_list_get()));
  for(int i = 0; i < 200; ++i)
    {
      linked_list_append();
    }
  CU_ASSERT_EQUAL(200, linked_list_size());
  CU_ASSERT_EQUAL(201, ioopm_linked_list_size(linked_list_get())); // The list + all 200 links
  release(list);
  CU_ASSERT_EQUAL(101, linked_list_size()); // The list + 99 links were deallocated
  CU_ASSERT_EQUAL(101, ioopm_linked_list_size(linked_list_get()));
  char *str = allocate(sizeof(char *), NULL);
  CU_ASSERT_EQUAL(1, linked_list_size()); // Calling allocate should remove 100 links using the the cascade list, which leaves 1 left in the list
  CU_ASSERT_EQUAL(2, ioopm_linked_list_size(linked_list_get())); // All objects except str, which we just allocated, and the last link in the list should have been deallocated

  deallocate(str);
  shutdown();
}

void test_scuffed_table()
{
  hash_t *ht = demo_hash_table_create();
  retain(ht);
  size_t size = demo_hash_table_size(ht);
  CU_ASSERT_EQUAL(size, 0);
  for(int i= 0; i < 100; ++i)
    {
      demo_hash_table_insert(ht, i, "Xmas");
    }
  size = demo_hash_table_size(ht);
  CU_ASSERT_EQUAL(size, 100);
  demo_hash_table_remove(ht, 10);
  demo_hash_table_remove(ht, 11);
  size = demo_hash_table_size(ht);
  CU_ASSERT_EQUAL(size,98);
  release(ht);
  shutdown();
}

void test_cascade_no_limit() //Test with using alloc after cascade limit is reached
{
  size_reset(); /// Resets the size of test size counter
  list_t *list = list_create();
  retain(list);
  set_cascade_limit(0);
  for(int i = 0; i < 543; ++i)
    {
      linked_list_append();
    }
  release(list);
  CU_ASSERT_EQUAL(0,linked_list_size());
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
      (NULL == CU_add_test(test_suite1, "cleanup", test_cleanup))||
      (NULL == CU_add_test(test_suite1, "cleanup empty list", test_cleanup_empty))||
      (NULL == CU_add_test(test_suite1, "cleanup and deallocate", test_cleanup_and_deallocate))||
      (NULL == CU_add_test(test_suite1, "cleanup with retain", test_cleanup_retain))||
      (NULL == CU_add_test(test_suite1, "shutdown with allocs", test_shutdown_with_allocs))||
      (NULL == CU_add_test(test_suite1, "allocate different types", test_allocate_dif_structs))||
      (NULL == CU_add_test(test_suite1, "cascade free", test_cascade_free)) ||
      (NULL == CU_add_test(test_suite1, "default destructor", test_destruct_default))||
      (NULL == CU_add_test(test_suite1, "generic hash table",  test_scuffed_table)) ||
      (NULL == CU_add_test(test_suite1, "default destructor with many ptrs", test_destruct_default_several_ptrs))||
      (NULL == CU_add_test(test_suite1, "cascade dealloc after allocate", test_cascade_free_alloc))||
      (NULL == CU_add_test(test_suite1, "alloc with zero bytes", test_alloc_zero_bytes))||
      (NULL == CU_add_test(test_suite1, "alloc array with zero bytes", test_alloc_array_zero_bytes))||
      (NULL == CU_add_test(test_suite1, "retain overflow", test_retain_overflow))||
      (NULL == CU_add_test(test_suite1, "release underflow", test_release_underflow))||
      (NULL == CU_add_test(test_suite1, "cascade no limit", test_cascade_no_limit))||
      (NULL == CU_add_test(test_suite1, "cleanup different destructors", test_cleanup_dif_destructors))||
      (NULL == CU_add_test(test_suite1, "Default destructor for array", test_destruct_default_array))||
      (NULL == CU_add_test(test_suite1, "Alloc array struct", test_alloc_array_struct))
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
