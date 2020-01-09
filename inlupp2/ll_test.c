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

static elem_t remove_aux(list_t *list, int index, link_t *prev, link_t *elem)
{
  elem_t value = elem->value;

  prev->next = elem->next;
  link_destroy(elem);

  --list->list_size;
  
  return value;
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

//////////////////////////////// TESTS //////////////////////////////////////////////////

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

int main()
{
  CU_initialize_registry();
  
  CU_pSuite test_suite = CU_add_suite("Test the functions for inlupp_linked_list.c", NULL, NULL);

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

  CU_basic_set_mode(CU_BRM_VERBOSE);
  
  CU_basic_run_tests();

  CU_cleanup_registry(); 
  
  return CU_get_error();
}
