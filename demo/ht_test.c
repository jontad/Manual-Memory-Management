#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "linked_list.h"
#include "hash_table.h"
#include "common.h"


#define No_Buckets 17
#define Load 1


//////////////////////////// STATIC FUNCTIONS ///////////////////////////////////////////

static bool int_equiv(elem_t key_ignored, elem_t value, void *x)
{
  int elem = *(int *)x;
  return elem == value.ioopm_int; 
}

static bool check_length(elem_t key, elem_t value, void *extra)
{
  int *length = extra;
  return (strlen(value.ioopm_void_ptr) == *length);
}

/////////////////////////// TESTS//////////////////////////////////
void hash_table_test_insert_str(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);
  
  ioopm_hash_table_insert(ht, int_elem(1), str_elem("fresh key"));
  option_t o = ioopm_hash_table_lookup(ht, int_elem(1));
  CU_ASSERT_EQUAL(o.value.ioopm_str,"fresh key");
  
  ioopm_hash_table_insert(ht, int_elem(1), str_elem("replace key"));
  o = ioopm_hash_table_lookup(ht, int_elem(1));
  CU_ASSERT_EQUAL(o.value.ioopm_str,"replace key");

  ioopm_hash_table_insert(ht, int_elem(18), str_elem("additional key"));
  o = ioopm_hash_table_lookup(ht, int_elem(18));
  CU_ASSERT_EQUAL(o.value.ioopm_str,"additional key");

  ioopm_hash_table_insert(ht, int_elem(1371111), str_elem("large key"));
  o = ioopm_hash_table_lookup(ht, int_elem(1371111));
  CU_ASSERT_EQUAL(o.value.ioopm_str,"large key");
  
  ioopm_hash_table_destroy(ht);
}


void hash_table_test_lookup(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  option_t opt;
  for (int i = 0; i < (No_Buckets + 1); ++i)
    {
      opt = ioopm_hash_table_lookup(ht, int_elem(i));
      CU_ASSERT_EQUAL(opt.value.ioopm_void_ptr, NULL);
    }

  opt = ioopm_hash_table_lookup(ht, int_elem(35));
  CU_ASSERT_FALSE(opt.success);

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("insert"));
  opt = ioopm_hash_table_lookup(ht, int_elem(1));
  CU_ASSERT_EQUAL(opt.value.ioopm_str,"insert");

  ioopm_hash_table_destroy(ht);
}



void hash_table_test_remove(void)
{
  char *msg;
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);
  
  msg = ioopm_hash_table_remove(ht, int_elem(1)).ioopm_str;
  CU_ASSERT_STRING_EQUAL(msg, "Non existent key");

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("insert"));
  msg = ioopm_hash_table_remove(ht, int_elem(1)).ioopm_str;
  CU_ASSERT_EQUAL(msg, "insert");

  ioopm_hash_table_destroy(ht);
}


void hash_table_test_size(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);
 
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);

  for (int i = 0; i < 10; ++i) {
    ioopm_hash_table_insert(ht, int_elem(i), str_elem("test"));
  }

  int size = ioopm_hash_table_size(ht);
  
  CU_ASSERT_EQUAL(size, 10);
  
  ioopm_hash_table_destroy(ht);
}


void hash_table_test_is_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("inserted value"));
  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));
  
  
  ioopm_hash_table_destroy(ht);
}

void hash_table_test_clear(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("inserted value"));
  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}


void hash_table_test_keys(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  for (int i = 0; i < 10; ++i)
    {
      ioopm_hash_table_insert(ht, int_elem(i), str_elem("hej"));
    }

  ioopm_list_t *list = ioopm_hash_table_keys(ht);

  for (int i = 0; i < 10; ++i)
    {
      CU_ASSERT_EQUAL(ioopm_linked_list_get(list, i).ioopm_int, i);
    }
  
  
  ioopm_hash_table_destroy(ht);
  ioopm_linked_list_destroy(list);
}


void hash_table_test_values(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("hej"));
  ioopm_hash_table_insert(ht, int_elem(2), str_elem("på"));
  ioopm_hash_table_insert(ht, int_elem(3), str_elem("dig"));
  ioopm_hash_table_insert(ht, int_elem(4), str_elem("jag"));
  ioopm_hash_table_insert(ht, int_elem(5), str_elem("gillar"));
  ioopm_hash_table_insert(ht, int_elem(6), str_elem("kaffe"));
  ioopm_hash_table_insert(ht, int_elem(7), str_elem("från"));
  ioopm_hash_table_insert(ht, int_elem(8), str_elem("skrubben"));
  ioopm_hash_table_insert(ht, int_elem(9), str_elem("som"));
  ioopm_hash_table_insert(ht, int_elem(10), str_elem("bränns"));

  ioopm_list_t *arr = ioopm_hash_table_values(ht);

  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 0), str_elem("hej")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 1), str_elem("på")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 2), str_elem("dig")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 3), str_elem("jag")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 4), str_elem("gillar")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 5), str_elem("kaffe")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 6), str_elem("från")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 7), str_elem("skrubben")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 8), str_elem("som")));
  CU_ASSERT_TRUE(equality_function_str(ioopm_linked_list_get(arr, 9), str_elem("bränns")));

  ioopm_linked_list_destroy(arr);
  ioopm_hash_table_destroy(ht);
}


// Tests both keys, values and ioopm_hash_table_any().
void hash_table_test_has_key(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  for (int i = 0; i < 10; ++i)
    {
      ioopm_hash_table_insert(ht, int_elem(i), str_elem("hej"));
      CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, int_elem(i)));
    }

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, int_elem(76)));
  
  ioopm_hash_table_destroy(ht);
}


void hash_table_test_has_value_str(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  char *str = "hej";
  
  ioopm_hash_table_insert(ht, int_elem(1), str_elem(str));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, str_elem(str)));

  
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, str_elem("hej")));
  

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, str_elem("foo")));

  ioopm_hash_table_destroy(ht);
}

void hash_table_test_has_value_int(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_int, 1, 17);

  int i = 1;
  
  ioopm_hash_table_insert(ht, str_elem("foo"), int_elem(i));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, int_elem(i)));

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, int_elem(23)));

  ioopm_hash_table_destroy(ht);
}


void hash_table_test_all_str(void)
{
 
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 0.75, 17);
  ioopm_hash_table_insert(ht, int_elem(2), str_elem("hej"));
  ioopm_hash_table_insert(ht, int_elem(3), str_elem("yey"));
  ioopm_hash_table_insert(ht, int_elem(20), str_elem("lol"));
  int test_number = 3;
  bool result = ioopm_hash_table_all(ht, check_length, &test_number);
  CU_ASSERT_TRUE(result); 

  test_number = 4;
  result = ioopm_hash_table_all(ht, check_length, &test_number);
  CU_ASSERT_FALSE(result);
  
  ioopm_hash_table_destroy(ht);
}


void hash_table_test_all_int(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_int, 1, 17);

  ioopm_hash_table_insert(ht, str_elem("a"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("b"), int_elem(2));
  ioopm_hash_table_insert(ht, str_elem("c"), int_elem(2));
  ioopm_hash_table_insert(ht, str_elem("d"), int_elem(3));
  ioopm_hash_table_insert(ht, str_elem("e"), int_elem(4));
  ioopm_hash_table_insert(ht, str_elem("f"), int_elem(5));
  ioopm_hash_table_insert(ht, str_elem("g"), int_elem(6));
  ioopm_hash_table_insert(ht, str_elem("h"), int_elem(7));
  ioopm_hash_table_insert(ht, str_elem("i"), int_elem(8));
  int number = 1;
  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, int_equiv, &number));
  
  ioopm_hash_table_insert(ht, str_elem("b"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("c"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("d"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("e"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("f"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("g"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("h"), int_elem(1));
  ioopm_hash_table_insert(ht, str_elem("i"), int_elem(1));
  
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, int_equiv, &number));
  
  ioopm_hash_table_destroy(ht);
}

static void change_value(elem_t key, elem_t *value, void *extra)
{
  ioopm_hash_table_t *ht = extra;
  ioopm_hash_table_insert(ht, key, str_elem("hello"));
}


void hash_table_test_apply_to_all(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 0.75, 17);
  ioopm_hash_table_insert(ht, int_elem(1), str_elem("hej"));
  ioopm_hash_table_insert(ht, int_elem(3), str_elem("yeye"));
  ioopm_hash_table_insert(ht, int_elem(20), str_elem("lolo"));
  ioopm_hash_table_apply_to_all(ht, change_value, ht);
  ioopm_list_t *values = ioopm_hash_table_values(ht);
  CU_ASSERT_STRING_EQUAL(ioopm_linked_list_get(values, 0).ioopm_void_ptr, "hello");
  CU_ASSERT_STRING_EQUAL(ioopm_linked_list_get(values, 1).ioopm_void_ptr, "hello");
  CU_ASSERT_STRING_EQUAL(ioopm_linked_list_get(values, 2).ioopm_void_ptr, "hello");

  ioopm_linked_list_destroy(values);
  ioopm_hash_table_destroy(ht);
}


void hash_table_test_adaptive_buckets(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  for (int i = 0; i < 20; ++i) {
    ioopm_hash_table_insert(ht, int_elem(i), str_elem("hej"));
  }

  CU_ASSERT_EQUAL(ht->capacity, 31);
  
  ioopm_hash_table_destroy(ht);
}


int main()
{
  CU_initialize_registry();
  
  CU_pSuite test_suite = CU_add_suite("Test the functions for hash_table.c", NULL, NULL);

  CU_add_test(test_suite, "Insert (Str)", hash_table_test_insert_str);
  CU_add_test(test_suite, "Lookup", hash_table_test_lookup);
  CU_add_test(test_suite, "Remove", hash_table_test_remove); 
  CU_add_test(test_suite, "Size", hash_table_test_size);
  CU_add_test(test_suite, "Empty", hash_table_test_is_empty);
  CU_add_test(test_suite, "Clear", hash_table_test_clear);
  CU_add_test(test_suite, "Keys", hash_table_test_keys);
  CU_add_test(test_suite, "Values", hash_table_test_values); 
  CU_add_test(test_suite, "Has key", hash_table_test_has_key);
  CU_add_test(test_suite, "Has value (Str)", hash_table_test_has_value_str);
  CU_add_test(test_suite, "Has value (Int)", hash_table_test_has_value_int);
  CU_add_test(test_suite, "All (Int)", hash_table_test_all_int);
  CU_add_test(test_suite, "All (Str)", hash_table_test_all_str);
 

  CU_add_test(test_suite, "Apply to all", hash_table_test_apply_to_all);

  CU_add_test(test_suite, "Adaptive buckets", hash_table_test_adaptive_buckets);
  CU_basic_run_tests();

  CU_cleanup_registry(); 
  
  return 0;
}
