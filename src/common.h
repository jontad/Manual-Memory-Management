#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>

/**
 * @file common.h
 * @author Jonathan Gustafsson, Daniel Westberg
 * @date 14 Oct 2019
 * @brief All definitions, type definitions, unions and structs used in the source files.
 */

/**************** COMMON DEFINITIONS ****************/
#define Success(v)      (option_t) { .success = true, .value = v };
#define Failure()       (option_t) { .success = false };
#define Successful(o)   (o.success == true)
#define Unsuccessful(o) (o.success == false)
#define int_elem(i) (elem_t) { .int_val=(i) }
#define str_elem(s) (elem_t) { .str_val=(s) }
#define obj_elem(o) (elem_t) { .obj_val=(o) }

/**************** COMMON TYPE DEFINITIONS ****************/
/// An element
typedef union elem elem_t;
// Option
typedef struct option option_t;

/// @brief 
typedef void obj;

/// @brief
/// @param
typedef void(*function1_t)(obj *);

/// @brief function for comparing two elements
/// @param a first element
/// @param a second element
/// @return true if the elements are equal, otherwise false
typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

/// @brief Generic function for comparing key or element with extra
/// @param key key to be compared with
/// @param value value to be compared with
/// @param extra generic value to be compared with
/// @return true if the statement is correct, otherwise false
typedef bool (*ioopm_predicate)(elem_t key, elem_t value, void *extra);

/// @brief generic function for applying a function to key or value with extra
/// @param key key to be applied
/// @param value value to be applied
/// @param extra generic value to use for application
typedef void (*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);


/**************** COMMON UNIONS ****************/

union elem
{
  int int_val;
  unsigned int uns_int_val;
  bool bool_val;
  float float_val;
  char *str_val;
  void *void_val;
  obj *obj_val;
};

/**************** COMMON STRUCTS ****************/

struct option
{
  bool success;
  elem_t value;
};


/**************** HASH TABLE TYPE DEFINITIONS ****************/

/// An entry in the hash table
typedef struct entry entry_t;
/// A hash table
typedef struct hash_table ioopm_hash_table_t;

/// @brief Hash function
/// @param key key to use the function on
/// @return bucket where the element is to be stored
typedef int(*hash_func)(elem_t key);


/**************** HASH TABLE STRUCTS ****************/

struct entry
{
  elem_t key;
  elem_t value;
  entry_t *next;
};

struct hash_table
{
  entry_t *buckets;
  hash_func h_func;
  ioopm_eq_function cmp_func;
  size_t size;
  size_t no_buckets;
  double load_factor;
};



/**************** LINKED LIST TYPE DEFINITIONS ****************/

/// A link in the linked structure
typedef struct link link_t;
// A list
typedef struct list ioopm_list_t;


/**************** LINKED LIST STRUCTS ****************/

struct link
{
  elem_t element;
  link_t *next;
};

struct list
{
  link_t *first;
  link_t *last;
  size_t size;
  ioopm_eq_function eq_fun;
};


/**************** ITERATOR TYPE DEFINITIONS ****************/

// Iterator
typedef struct iter ioopm_list_iterator_t;

/**************** ITERATOR STRUCTS ****************/

struct iter
{
    link_t *current;
    ioopm_list_t *list;
};


bool eq_func(elem_t a, elem_t b);
ioopm_list_t *get_cascade_list();
void set_cascade_list_to_null();
#endif
