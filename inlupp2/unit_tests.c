#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "inlupp_linked_list.h"
#include "hash_table.h"
#include "iterator.h"
#include "common.h"


#define No_Buckets 17
#define Load 1

struct iter
{
  link_t *current;
  list_t *list;
};

//////////////////////////// STATIC FUNCTIONS ///////////////////////////////////////////


static bool check_func(link_t *element, void *extra)
{
  return ((element->value).int1 == 0);
}

static void apply_func(link_t **element, void *extra)
{
  ((*element)->value) = int_elem(0);
}

static void link_destroy(link_t *link)
{
  Free(link);
}

static bool int_equiv(elem_t key_ignored, elem_t value, void *x)
{
  int elem = *(int *)x;
  return elem == value.int1; 
}

static elem_t remove_aux(list_t *list, int index, link_t *prev, link_t *elem)
{
  elem_t value = elem->value;

  prev->next = elem->next;
  link_destroy(elem);

  --list->list_size;
  
  return value;
}

static bool check_length(elem_t key, elem_t value, void *extra)
{
  int *length = extra;
  return (strlen(value.void_ptr) == *length);
}

static elem_t test_iterator_func(list_t *list, int index)
{
  list_iterator_t *iter = list_iterator(list);
  for(int i = 0; i<index; ++i)
    {
      if(!iterator_has_next(iter))
	{
	  break;
	}
      iterator_next(iter);
    }

  link_t *elem = iter->current;
  
  iterator_reset(iter);
  for(int i = 0; i<index-1; ++i)
    {
      if(!iterator_has_next(iter))
	{
	  break;
	}
      iterator_next(iter);
    }
  
  
  
  link_t *prev = iter->current;
  elem_t value;
  
  if (index == 0)
    {
      value = prev->value;
      list->first = prev->next;
      link_destroy(prev);
      --list->list_size;
    }
  else if(elem != NULL)
    {
      value = remove_aux(list, index, prev, elem);
    }
  iterator_destroy(iter);
  return value;
}

// TESTS //////////////////////////////////////////////////////////////////////////////
void test_inlupp_linked_list_get(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(13));
  int i = inlupp_linked_list_get(ll, 0).int1;
  CU_ASSERT_EQUAL(i, 13);

  
  inlupp_linked_list_insert(ll, 7, int_elem(14));
  i = inlupp_linked_list_get(ll, 1).int1;
  CU_ASSERT_EQUAL(i,14);

  inlupp_linked_list_insert(ll, 8, int_elem(15));
  i = inlupp_linked_list_get(ll, 2).int1;
  CU_ASSERT_EQUAL(i,15);
  
    
  inlupp_linked_list_destroy(ll);
}




void test_inlupp_linked_list_insert(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_insert(ll, 5, int_elem(10));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 10);

  
  inlupp_linked_list_insert(ll, 1, int_elem(1));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 1).int1, 1);
  
  inlupp_linked_list_insert(ll, 0, int_elem(5));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 5);

  inlupp_linked_list_insert(ll, 35, int_elem(27));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 3).int1, 27);

  inlupp_linked_list_insert(ll, -17, int_elem(99));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 99);
  
  inlupp_linked_list_destroy(ll);
}


void test_inlupp_linked_list_prepend(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  CU_ASSERT_PTR_NULL(ll->first);
  inlupp_linked_list_prepend(ll, int_elem(10));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 10);

  
  inlupp_linked_list_prepend(ll, int_elem(100));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 100);

  
  inlupp_linked_list_prepend(ll, int_elem(1000));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 1000);
  
  inlupp_linked_list_destroy(ll);
}


void test_inlupp_linked_list_append(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(10));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 10);

  
  inlupp_linked_list_append(ll, int_elem(100));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 1).int1, 100);

  inlupp_linked_list_append(ll, int_elem(1000));
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 2).int1, 1000);
  
  
  inlupp_linked_list_destroy(ll);
}


void test_inlupp_linked_list_remove(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(10));
  inlupp_linked_list_append(ll, int_elem(100));
  inlupp_linked_list_append(ll, int_elem(1000));
  
  inlupp_linked_list_remove(ll, 1);
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 1).int1, 1000);  

  
  inlupp_linked_list_remove(ll, 0);
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 1000);  
  
  inlupp_linked_list_remove(ll, 0);
  CU_ASSERT_PTR_NULL(ll->first);  
  
  inlupp_linked_list_destroy(ll);
}


void test_inlupp_linked_list_contains(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  CU_ASSERT_FALSE(inlupp_linked_list_contains(ll, int_elem(10)));

  inlupp_linked_list_append(ll, int_elem(10));
  CU_ASSERT_TRUE(inlupp_linked_list_contains(ll, int_elem(10)));
  		 
  inlupp_linked_list_append(ll, int_elem(100));
  //inlupp_linked_list_append(ll, int_elem(1000));
  //CU_ASSERT_TRUE(inlupp_linked_list_contains(ll, int_elem(1000)));
  
  inlupp_linked_list_destroy(ll);
}



void test_inlupp_linked_list_size(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  size_t i = inlupp_linked_list_size(ll);
  CU_ASSERT_EQUAL(i, 0);
  
  inlupp_linked_list_append(ll, int_elem(0));
  inlupp_linked_list_append(ll, int_elem(0));
  inlupp_linked_list_append(ll, int_elem(0));
  
  i = inlupp_linked_list_size(ll);
  CU_ASSERT_EQUAL(i,3);

  inlupp_linked_list_destroy(ll);
}

void test_inlupp_linked_list_is_empty(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);
  CU_ASSERT_TRUE(inlupp_linked_list_is_empty(ll));

  inlupp_linked_list_append(ll, int_elem(10));
  inlupp_linked_list_append(ll, int_elem(100));
  inlupp_linked_list_append(ll, int_elem(1000));
  CU_ASSERT_FALSE(inlupp_linked_list_is_empty(ll));

  inlupp_linked_list_destroy(ll);
}


void test_inlupp_linked_list_all(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(0));
  inlupp_linked_list_append(ll, int_elem(0));
  inlupp_linked_list_insert(ll, 1, int_elem(0));
  
  CU_ASSERT_TRUE(inlupp_linked_list_all(ll, check_func, NULL));

  inlupp_linked_list_append(ll, int_elem(10));

  CU_ASSERT_FALSE(inlupp_linked_list_all(ll, check_func, NULL));
  
  inlupp_linked_list_destroy(ll);
}

void test_inlupp_linked_list_any(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(10));
  inlupp_linked_list_append(ll, int_elem(1000));
  inlupp_linked_list_append(ll, int_elem(1100));
  
  CU_ASSERT_FALSE(inlupp_linked_list_any(ll, check_func, NULL));

  inlupp_linked_list_append(ll, int_elem(0));

  CU_ASSERT_TRUE(inlupp_linked_list_any(ll, check_func, NULL));
  
  inlupp_linked_list_destroy(ll);
}


void test_inlupp_linked_list_apply_to_all(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(10));
  inlupp_linked_list_append(ll, int_elem(1000));
  inlupp_linked_list_append(ll, int_elem(1100));
  CU_ASSERT_FALSE(inlupp_linked_list_any(ll, check_func, NULL));

  inlupp_linked_apply_to_all(ll, apply_func, NULL);
  CU_ASSERT_TRUE(inlupp_linked_list_any(ll, check_func, NULL));

  inlupp_linked_list_destroy(ll);
}


void test_iterator(void)
{
  list_t *ll = inlupp_linked_list_create(equality_function_int);

  inlupp_linked_list_append(ll, int_elem(10));
  inlupp_linked_list_append(ll, int_elem(100));
  inlupp_linked_list_append(ll, int_elem(1000));

  test_iterator_func(ll, 1);
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 1).int1, 1000);  
  
  test_iterator_func(ll, 0);
  CU_ASSERT_EQUAL(inlupp_linked_list_get(ll, 0).int1, 1000);  

  test_iterator_func(ll, 0);
  CU_ASSERT_PTR_NULL(ll->first);  

  inlupp_linked_list_destroy(ll);
}





void test_hash_table_insert_str(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);
  
  hash_table_insert(ht, int_elem(1), str_elem("fresh key"));
  option_t o = hash_table_lookup(ht, int_elem(1));
  CU_ASSERT_EQUAL(o.value.str,"fresh key");
  
  hash_table_insert(ht, int_elem(1), str_elem("replace key"));
  o = hash_table_lookup(ht, int_elem(1));
  CU_ASSERT_EQUAL(o.value.str,"replace key");

  hash_table_insert(ht, int_elem(18), str_elem("additional key"));
  o = hash_table_lookup(ht, int_elem(18));
  CU_ASSERT_EQUAL(o.value.str,"additional key");

  hash_table_insert(ht, int_elem(1371111), str_elem("large key"));
  o = hash_table_lookup(ht, int_elem(1371111));
  CU_ASSERT_EQUAL(o.value.str,"large key");
  
  hash_table_destroy(ht);
}


void test_hash_table_lookup(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  option_t opt;
  for (int i = 0; i < (No_Buckets + 1); ++i)
    {
      opt = hash_table_lookup(ht, int_elem(i));
      CU_ASSERT_EQUAL(opt.value.void_ptr, NULL);
    }

  opt = hash_table_lookup(ht, int_elem(35));
  CU_ASSERT_FALSE(opt.success);

  hash_table_insert(ht, int_elem(1), str_elem("insert"));
  opt = hash_table_lookup(ht, int_elem(1));
  CU_ASSERT_EQUAL(opt.value.str,"insert");

  hash_table_destroy(ht);
}



void test_hash_table_remove(void)
{
  char *msg;
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);
  
  msg = hash_table_remove(ht, int_elem(1)).str;
  CU_ASSERT_STRING_EQUAL(msg, "Non existent key");

  hash_table_insert(ht, int_elem(1), str_elem("insert"));
  msg = hash_table_remove(ht, int_elem(1)).str;
  CU_ASSERT_EQUAL(msg, "insert");

  hash_table_destroy(ht);
}


void test_hash_table_size(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);
 
  CU_ASSERT_EQUAL(hash_table_size(ht), 0);

  for (int i = 0; i < 10; ++i) {
    hash_table_insert(ht, int_elem(i), str_elem("test"));
  }

  int size = hash_table_size(ht);
  
  CU_ASSERT_EQUAL(size, 10);
  
  hash_table_destroy(ht);
}


void test_hash_table_is_empty(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  CU_ASSERT_TRUE(hash_table_is_empty(ht));

  hash_table_insert(ht, int_elem(1), str_elem("inserted value"));
  CU_ASSERT_FALSE(hash_table_is_empty(ht));
  
  
  hash_table_destroy(ht);
}

void test_hash_table_clear(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  hash_table_clear(ht);
  CU_ASSERT_TRUE(hash_table_is_empty(ht));

  hash_table_insert(ht, int_elem(1), str_elem("inserted value"));
  hash_table_clear(ht);
  CU_ASSERT_TRUE(hash_table_is_empty(ht));

  hash_table_destroy(ht);
}


void test_hash_table_keys(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  for (int i = 0; i < 10; ++i)
    {
      hash_table_insert(ht, int_elem(i), str_elem("hej"));
    }

  list_t *list = hash_table_keys(ht);

  for (int i = 0; i < 10; ++i)
    {
      CU_ASSERT_EQUAL(inlupp_linked_list_get(list, i).int1, i);
    }
  
  
  hash_table_destroy(ht);
  inlupp_linked_list_destroy(list);
}


void test_hash_table_values(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  hash_table_insert(ht, int_elem(1), str_elem("hej"));
  hash_table_insert(ht, int_elem(2), str_elem("på"));
  hash_table_insert(ht, int_elem(3), str_elem("dig"));
  hash_table_insert(ht, int_elem(4), str_elem("jag"));
  hash_table_insert(ht, int_elem(5), str_elem("gillar"));
  hash_table_insert(ht, int_elem(6), str_elem("kaffe"));
  hash_table_insert(ht, int_elem(7), str_elem("från"));
  hash_table_insert(ht, int_elem(8), str_elem("skrubben"));
  hash_table_insert(ht, int_elem(9), str_elem("som"));
  hash_table_insert(ht, int_elem(10), str_elem("bränns"));

  list_t *arr = hash_table_values(ht);

  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 0), str_elem("hej")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 1), str_elem("på")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 2), str_elem("dig")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 3), str_elem("jag")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 4), str_elem("gillar")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 5), str_elem("kaffe")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 6), str_elem("från")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 7), str_elem("skrubben")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 8), str_elem("som")));
  CU_ASSERT_TRUE(equality_function_str(inlupp_linked_list_get(arr, 9), str_elem("bränns")));

  inlupp_linked_list_destroy(arr);
  hash_table_destroy(ht);
}


// Tests both keys, values and hash_table_any().
void test_hash_table_has_key(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  for (int i = 0; i < 10; ++i)
    {
      hash_table_insert(ht, int_elem(i), str_elem("hej"));
      CU_ASSERT_TRUE(hash_table_has_key(ht, int_elem(i)));
    }

  CU_ASSERT_FALSE(hash_table_has_key(ht, int_elem(76)));
  
  hash_table_destroy(ht);
}


void test_hash_table_has_value_str(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  char *str = "hej";
  
  hash_table_insert(ht, int_elem(1), str_elem(str));
  CU_ASSERT_TRUE(hash_table_has_value(ht, str_elem(str)));

  
  CU_ASSERT_TRUE(hash_table_has_value(ht, str_elem("hej")));
  

  CU_ASSERT_FALSE(hash_table_has_value(ht, str_elem("foo")));

  hash_table_destroy(ht);
}

void test_hash_table_has_value_int(void)
{
  hash_table_t *ht = hash_table_create(str_hash_func, equality_function_str, equality_function_int, 1, 17);

  int i = 1;
  
  hash_table_insert(ht, str_elem("foo"), int_elem(i));
  CU_ASSERT_TRUE(hash_table_has_value(ht, int_elem(i)));

  CU_ASSERT_FALSE(hash_table_has_value(ht, int_elem(23)));

  hash_table_destroy(ht);
}


void test_hash_table_all_str(void)
{
 
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 0.75, 17);
  hash_table_insert(ht, int_elem(2), str_elem("hej"));
  hash_table_insert(ht, int_elem(3), str_elem("yey"));
  hash_table_insert(ht, int_elem(20), str_elem("lol"));
  int test_number = 3;
  bool result = hash_table_all(ht, check_length, &test_number);
  CU_ASSERT_TRUE(result); 

  test_number = 4;
  result = hash_table_all(ht, check_length, &test_number);
  CU_ASSERT_FALSE(result);
  
  hash_table_destroy(ht);
}


void test_hash_table_all_int(void)
{
  hash_table_t *ht = hash_table_create(str_hash_func, equality_function_str, equality_function_int, 1, 17);

  hash_table_insert(ht, str_elem("a"), int_elem(1));
  hash_table_insert(ht, str_elem("b"), int_elem(2));
  hash_table_insert(ht, str_elem("c"), int_elem(2));
  hash_table_insert(ht, str_elem("d"), int_elem(3));
  hash_table_insert(ht, str_elem("e"), int_elem(4));
  hash_table_insert(ht, str_elem("f"), int_elem(5));
  hash_table_insert(ht, str_elem("g"), int_elem(6));
  hash_table_insert(ht, str_elem("h"), int_elem(7));
  hash_table_insert(ht, str_elem("i"), int_elem(8));
  int number = 1;
  CU_ASSERT_FALSE(hash_table_all(ht, int_equiv, &number));
  
  hash_table_insert(ht, str_elem("b"), int_elem(1));
  hash_table_insert(ht, str_elem("c"), int_elem(1));
  hash_table_insert(ht, str_elem("d"), int_elem(1));
  hash_table_insert(ht, str_elem("e"), int_elem(1));
  hash_table_insert(ht, str_elem("f"), int_elem(1));
  hash_table_insert(ht, str_elem("g"), int_elem(1));
  hash_table_insert(ht, str_elem("h"), int_elem(1));
  hash_table_insert(ht, str_elem("i"), int_elem(1));
  
  CU_ASSERT_TRUE(hash_table_all(ht, int_equiv, &number));
  
  hash_table_destroy(ht);
}

static void change_value(elem_t key, elem_t *value, void *extra)
{
  hash_table_t *ht = extra;
  hash_table_insert(ht, key, str_elem("hello"));
}


void test_hash_table_apply_to_all(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 0.75, 17);
  hash_table_insert(ht, int_elem(1), str_elem("hej"));
  hash_table_insert(ht, int_elem(3), str_elem("yeye"));
  hash_table_insert(ht, int_elem(20), str_elem("lolo"));
  hash_table_apply_to_all(ht, change_value, ht);
  list_t *values = hash_table_values(ht);
  CU_ASSERT_STRING_EQUAL(inlupp_linked_list_get(values, 0).void_ptr, "hello");
  CU_ASSERT_STRING_EQUAL(inlupp_linked_list_get(values, 1).void_ptr, "hello");
  CU_ASSERT_STRING_EQUAL(inlupp_linked_list_get(values, 2).void_ptr, "hello");

  inlupp_linked_list_destroy(values);
  hash_table_destroy(ht);
}


void test_hash_table_adaptive_buckets(void)
{
  hash_table_t *ht = hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  for (int i = 0; i < 20; ++i) {
    hash_table_insert(ht, int_elem(i), str_elem("hej"));
  }

  CU_ASSERT_EQUAL(ht->capacity, 31);
  
  hash_table_destroy(ht);
}


int main()
{
  CU_initialize_registry();
  
  CU_pSuite test_suite = CU_add_suite("Test the functions for hash_table.c and inlupp_linked_list.c", NULL, NULL);

  CU_add_test(test_suite, "Insert", test_inlupp_linked_list_insert);
  CU_add_test(test_suite, "Get", test_inlupp_linked_list_get);
  CU_add_test(test_suite, "Prepend", test_inlupp_linked_list_prepend);
  CU_add_test(test_suite, "Append", test_inlupp_linked_list_append);
  CU_add_test(test_suite, "Remove", test_inlupp_linked_list_remove);
  CU_add_test(test_suite, "Contains", test_inlupp_linked_list_contains);
  CU_add_test(test_suite, "Size", test_inlupp_linked_list_size);
  CU_add_test(test_suite, "Empty", test_inlupp_linked_list_is_empty);
  CU_add_test(test_suite, "All", test_inlupp_linked_list_all);
  CU_add_test(test_suite, "Any", test_inlupp_linked_list_any); 
  CU_add_test(test_suite, "Apply", test_inlupp_linked_list_apply_to_all);
  CU_add_test(test_suite, "Iterator", test_iterator);

  CU_add_test(test_suite, "Insert (Str)", test_hash_table_insert_str);
  CU_add_test(test_suite, "Lookup", test_hash_table_lookup);
  CU_add_test(test_suite, "Remove", test_hash_table_remove); //leak (still-reachable)
  CU_add_test(test_suite, "Size", test_hash_table_size);
  CU_add_test(test_suite, "Empty", test_hash_table_is_empty);
  CU_add_test(test_suite, "Clear", test_hash_table_clear);
  CU_add_test(test_suite, "Keys", test_hash_table_keys);
  CU_add_test(test_suite, "Values", test_hash_table_values); 
  CU_add_test(test_suite, "Has key", test_hash_table_has_key);
  CU_add_test(test_suite, "Has value (Str)", test_hash_table_has_value_str);
  CU_add_test(test_suite, "Has value (Int)", test_hash_table_has_value_int);
  CU_add_test(test_suite, "All (Int)", test_hash_table_all_int);
  CU_add_test(test_suite, "All (Str)", test_hash_table_all_str);
 

  CU_add_test(test_suite, "Apply to all", test_hash_table_apply_to_all);


  CU_add_test(test_suite, "Adaptive buckets", test_hash_table_adaptive_buckets);
  CU_basic_run_tests();

  CU_cleanup_registry(); 
  
  return 0;
}
