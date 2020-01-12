#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "linked_list.h"
#include "hash_table.h"
#include "iterator.h"
#include "common.h"


#define No_Buckets 17
#define Load 1

#define int_elem(i) (elem_t) {.ioopm_int=(i)}
#define str_elem(s) (elem_t) {.ioopm_str=(s)}
#define bool_elem(b) (elem_t) {.ioopm_bool=(b)}
#define void_elem(v) (elem_t) {.ioopm_void_ptr=(v)}


#define Free(ptr) {free(ptr); ptr = NULL;}

// STRUCTS ////////////////////////////////////////////////////////////////////////////
struct link 
{
  elem_t value;
  ioopm_link_t *next;
};

struct hash_table
{
  float load_factor;
  size_t capacity;
  entry_t **buckets;
  hash_function hash_func;
  ioopm_eq_function key_eq_func;
  ioopm_eq_function value_eq_func;
  size_t size;
};

struct list
{
  size_t list_size;
  ioopm_eq_function equal;
  ioopm_link_t *first;
  ioopm_link_t *last;
};

struct iter
{
  ioopm_link_t *current;
  ioopm_list_t *list;
};

struct entry
{
  elem_t key;
  elem_t value;
  entry_t *next;
};


struct option
{
  bool success;
  elem_t value;
};

// STATIC FUNCTIONS ///////////////////////////////////////////////////////////////////


static bool check_func(ioopm_link_t *element, void *extra)
{
  return ((element->value).ioopm_int == 0);
}

static void apply_func(ioopm_link_t **element, void *extra)
{
  ((*element)->value) = int_elem(0);
}

static void link_destroy(ioopm_link_t *link)
{
  Free(link);
}

static bool int_equiv(elem_t key_ignored, elem_t value, void *x)
{
  int elem = *(int *)x;
  return elem == value.ioopm_int; 
}

static elem_t remove_aux(ioopm_list_t *list, int index, ioopm_link_t *prev, ioopm_link_t *elem)
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
  return (strlen(value.ioopm_void_ptr) == *length);
}

static elem_t test_iterator_func(ioopm_list_t *list, int index)
{
  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  for(int i = 0; i<index; ++i)
    {
      if(!ioopm_iterator_has_next(iter))
	{
	  break;
	}
      ioopm_iterator_next(iter);
    }

  ioopm_link_t *elem = iter->current;
  
  ioopm_iterator_reset(iter);
  for(int i = 0; i<index-1; ++i)
    {
      if(!ioopm_iterator_has_next(iter))
	{
	  break;
	}
      ioopm_iterator_next(iter);
    }
  
  
  
  ioopm_link_t *prev = iter->current;
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
  ioopm_iterator_destroy(iter);
  return value;
}

// TESTS //////////////////////////////////////////////////////////////////////////////
void test_linked_list_get(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(13));
  int i = ioopm_linked_list_get(ll, 0).ioopm_int;
  CU_ASSERT_EQUAL(i, 13);

  
  ioopm_linked_list_insert(ll, 7, int_elem(14));
  i = ioopm_linked_list_get(ll, 1).ioopm_int;
  CU_ASSERT_EQUAL(i,14);

  ioopm_linked_list_insert(ll, 8, int_elem(15));
  i = ioopm_linked_list_get(ll, 2).ioopm_int;
  CU_ASSERT_EQUAL(i,15);
  
    
  ioopm_linked_list_destroy(ll);
}




void test_linked_list_insert(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_insert(ll, 5, int_elem(10));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 10);

  
  ioopm_linked_list_insert(ll, 1, int_elem(1));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 1).ioopm_int, 1);
  
  ioopm_linked_list_insert(ll, 0, int_elem(5));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 5);

  ioopm_linked_list_insert(ll, 35, int_elem(27));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 3).ioopm_int, 27);

  ioopm_linked_list_insert(ll, -17, int_elem(99));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 99);
  
  ioopm_linked_list_destroy(ll);
}


void test_linked_list_prepend(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  CU_ASSERT_PTR_NULL(ll->first);
  ioopm_linked_list_prepend(ll, int_elem(10));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 10);

  
  ioopm_linked_list_prepend(ll, int_elem(100));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 100);

  
  ioopm_linked_list_prepend(ll, int_elem(1000));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 1000);
  
  ioopm_linked_list_destroy(ll);
}


void test_linked_list_append(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(10));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 10);

  
  ioopm_linked_list_append(ll, int_elem(100));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 1).ioopm_int, 100);

  ioopm_linked_list_append(ll, int_elem(1000));
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 2).ioopm_int, 1000);
  
  
  ioopm_linked_list_destroy(ll);
}


void test_linked_list_remove(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(10));
  ioopm_linked_list_append(ll, int_elem(100));
  ioopm_linked_list_append(ll, int_elem(1000));
  
  ioopm_linked_list_remove(ll, 1);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 1).ioopm_int, 1000);  

  
  ioopm_linked_list_remove(ll, 0);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 1000);  
  
  ioopm_linked_list_remove(ll, 0);
  CU_ASSERT_PTR_NULL(ll->first);  
  
  ioopm_linked_list_destroy(ll);
}


void test_linked_list_contains(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  CU_ASSERT_FALSE(ioopm_linked_list_contains(ll, int_elem(10)));

  ioopm_linked_list_append(ll, int_elem(10));
  CU_ASSERT_TRUE(ioopm_linked_list_contains(ll, int_elem(10)));
  		 
  ioopm_linked_list_append(ll, int_elem(100));
  ioopm_linked_list_append(ll, int_elem(1000));
  CU_ASSERT_TRUE(ioopm_linked_list_contains(ll, int_elem(1000)));
  
  ioopm_linked_list_destroy(ll);
}



void test_linked_list_size(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  size_t i = ioopm_linked_list_size(ll);
  CU_ASSERT_EQUAL(i, 0);
  
  ioopm_linked_list_append(ll, int_elem(0));
  ioopm_linked_list_append(ll, int_elem(0));
  ioopm_linked_list_append(ll, int_elem(0));
  
  i = ioopm_linked_list_size(ll);
  CU_ASSERT_EQUAL(i,3);

  ioopm_linked_list_destroy(ll);
}

void test_linked_list_is_empty(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);
  CU_ASSERT_TRUE(ioopm_linked_list_is_empty(ll));

  ioopm_linked_list_append(ll, int_elem(10));
  ioopm_linked_list_append(ll, int_elem(100));
  ioopm_linked_list_append(ll, int_elem(1000));
  CU_ASSERT_FALSE(ioopm_linked_list_is_empty(ll));

  ioopm_linked_list_destroy(ll);
}


void test_linked_list_all(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(0));
  ioopm_linked_list_append(ll, int_elem(0));
  ioopm_linked_list_insert(ll, 1, int_elem(0));
  
  CU_ASSERT_TRUE(ioopm_linked_list_all(ll, check_func, NULL));

  ioopm_linked_list_append(ll, int_elem(10));

  CU_ASSERT_FALSE(ioopm_linked_list_all(ll, check_func, NULL));
  
  ioopm_linked_list_destroy(ll);
}

void test_linked_list_any(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(10));
  ioopm_linked_list_append(ll, int_elem(1000));
  ioopm_linked_list_append(ll, int_elem(1100));
  
  CU_ASSERT_FALSE(ioopm_linked_list_any(ll, check_func, NULL));

  ioopm_linked_list_append(ll, int_elem(0));

  CU_ASSERT_TRUE(ioopm_linked_list_any(ll, check_func, NULL));
  
  ioopm_linked_list_destroy(ll);
}


void test_linked_list_apply_to_all(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(10));
  ioopm_linked_list_append(ll, int_elem(1000));
  ioopm_linked_list_append(ll, int_elem(1100));
  CU_ASSERT_FALSE(ioopm_linked_list_any(ll, check_func, NULL));

  ioopm_linked_apply_to_all(ll, apply_func, NULL);
  CU_ASSERT_TRUE(ioopm_linked_list_any(ll, check_func, NULL));

  ioopm_linked_list_destroy(ll);
}


void test_iterator(void)
{
  ioopm_list_t *ll = ioopm_linked_list_create(equality_function_int);

  ioopm_linked_list_append(ll, int_elem(10));
  ioopm_linked_list_append(ll, int_elem(100));
  ioopm_linked_list_append(ll, int_elem(1000));

  test_iterator_func(ll, 1);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 1).ioopm_int, 1000);  
  
  test_iterator_func(ll, 0);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(ll, 0).ioopm_int, 1000);  

  test_iterator_func(ll, 0);
  CU_ASSERT_PTR_NULL(ll->first);  

  ioopm_linked_list_destroy(ll);
}





void test_hash_table_insert_str(void)
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


void test_hash_table_lookup(void)
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



void test_hash_table_remove(void)
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


void test_hash_table_size(void)
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


void test_hash_table_is_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("inserted value"));
  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));
  
  
  ioopm_hash_table_destroy(ht);
}

void test_hash_table_clear(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_insert(ht, int_elem(1), str_elem("inserted value"));
  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}


void test_hash_table_keys(void)
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


void test_hash_table_values(void)
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
void test_hash_table_has_key(void)
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


void test_hash_table_has_value_str(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash_func, equality_function_int, equality_function_str, 1, 17);

  char *str = "hej";
  
  ioopm_hash_table_insert(ht, int_elem(1), str_elem(str));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, str_elem(str)));

  
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, str_elem("hej")));
  

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, str_elem("foo")));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_int(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_int, 1, 17);

  int i = 1;
  
  ioopm_hash_table_insert(ht, str_elem("foo"), int_elem(i));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, int_elem(i)));

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, int_elem(23)));

  ioopm_hash_table_destroy(ht);
}


void test_hash_table_all_str(void)
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


void test_hash_table_all_int(void)
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


void test_hash_table_apply_to_all(void)
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


void test_hash_table_adaptive_buckets(void)
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
  
  CU_pSuite test_suite = CU_add_suite("Test the functions for hash_table.c and linked_list.c", NULL, NULL);

  CU_add_test(test_suite, "Insert", test_linked_list_insert);
  CU_add_test(test_suite, "Get", test_linked_list_get);
  CU_add_test(test_suite, "Prepend", test_linked_list_prepend);
  CU_add_test(test_suite, "Append", test_linked_list_append);
  CU_add_test(test_suite, "Remove", test_linked_list_remove);
  CU_add_test(test_suite, "Contains", test_linked_list_contains);
  CU_add_test(test_suite, "Size", test_linked_list_size);
  CU_add_test(test_suite, "Empty", test_linked_list_is_empty);
  CU_add_test(test_suite, "All", test_linked_list_all);
  CU_add_test(test_suite, "Any", test_linked_list_any); 
  CU_add_test(test_suite, "Apply", test_linked_list_apply_to_all);
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


  //CU_add_test(test_suite, "Adaptive buckets", test_hash_table_adaptive_buckets);
  CU_basic_run_tests();

  CU_cleanup_registry(); 
  
  return 0;
}
