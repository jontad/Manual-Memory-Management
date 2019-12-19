#ifndef __LIB_FOR_TESTS_H__
#define __LIB_FOR_TESTS_H__
#include <stdbool.h>
#include "../src/refmem.h"
/**
 * @file lib_for_tests.h
 * @author Elias Insulander, Jonathan Tadese, Robert Paananen, Daniel Westberg, Alex Kangas, Georgios Davakos, Joel Waldenbäck
 * @date 13 December 2019
 * @brief Library for tests. Made to test allocating/deallocating memory for different data types and with different destructors.  
 *
 * More indepth explanation here of what the header defines
 * 
 * @see http://wrigstad.com/ioopm19/projects/project1.html
 */
typedef struct string_struct string_t;
typedef struct int_struct ourInt_t;
typedef struct linked_list list_t;
typedef struct new_link new_link_t;
typedef struct many_pointers ptr_t;
typedef struct bucket bucket_t;
typedef struct hash hash_t;

struct string_struct
{
  char *str;
};

struct many_pointers
{
  string_t *str_struct;
  ourInt_t *int_struct;
  char *str;
};

struct int_struct
{
  int k;
};

struct linked_list
{
  new_link_t *head;
  new_link_t *tail;
  size_t size;
};

struct new_link
{
  new_link_t *next;
  void *elem;
};

struct bucket
{
  int key;
  char *value;
  bucket_t* next;
};

struct hash
{
  int size;
  int num_buckets;
  bucket_t *buckets[];
};
/// @brief the following function is ment to deallocate a string
void destructor_string(obj *object);

void destructor_string_array(obj *object);

/// @brief the following function is ment to deallocate a linked list
void destructor_linked_list(obj *object);

void linked_list_append();

size_t linked_list_size();

void link_destructor(obj *c);

list_t *list_create();

void list_negate();

void size_reset();

hash_t *demo_hash_table_create();

void demo_hash_table_insert(hash_t *ht, int key, char *value);

void demo_hash_table_remove(hash_t *ht, int key);

size_t demo_hash_table_size(hash_t *ht);

void demo_hash_table_clear(hash_t *ht);
#endif
