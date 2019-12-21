#ifndef __LIB_FOR_TESTS_H__
#define __LIB_FOR_TESTS_H__
#include <stdbool.h>
#include "../src/refmem.h"
/**
 * @file lib_for_tests.h
 * @author Elias Insulander, Jonathan Tadese, Robert Paananen, Daniel Westberg, Alex Kangas, Georgios Davakos, Joel Waldenbäck
 * @date 20 December 2019
 * @brief Library of helper functions for tests. Made to test allocating/deallocating memory 
 * for different data types and with different destructors.  
 *
 * @see http://wrigstad.com/ioopm19/projects/project1.html
 */

/// @brief Renaming string_struct to string_t
typedef struct string_struct string_t;

/// @brief Renaming int_struct to ourInt_t
typedef struct int_struct ourInt_t;

/// @brief Renaming linked_list to list_t
typedef struct linked_list list_t;

/// @brief Renaming new_link to new_link_t
typedef struct new_link new_link_t;

/// @brief Renaming many_pointers to ptr_t
typedef struct many_pointers ptr_t;

/// @brief Renaming bucket to bucket_t
typedef struct bucket bucket_t;

/// @brief Renaming hash to hash_t
typedef struct hash hash_t;

/// @struct string_struct a string struct
/// @var str a string
struct string_struct
{
  char *str;
};

/// @struct many_pointers a struct that contains many different types of pointers
/// @var str_struct a pointer of a string
/// @var int_struct a pointer of an int
/// @var str pointer to a string
struct many_pointers
{
  string_t *str_struct;
  ourInt_t *int_struct;
  char *str;
};

/// @struct int_struct an int struct
/// @var k an int
struct int_struct
{
  int k;
};

/// @struct linked_list a generic linked list
/// @var head the first element of the linked list
/// @var tail the last element of the linked list
/// @var size the total number of elements in the list
struct linked_list
{
  new_link_t *head;
  new_link_t *tail;
  size_t size;
};

/// @struct new_link an element in a linked list
/// @var next a pointer to the next element in the linked list
/// @var elem pointer to the value of the element
struct new_link
{
  new_link_t *next;
  void *elem;
};

/// @struct bucket an entry inside a hash table
/// @var key the key of the entry
/// @var value the value of the entry
/// @var next pointer to the next entry
struct bucket
{
  int key;
  char *value;
  bucket_t* next;
};

/// @struct hash a generic hash table
/// @var size the total number of entries
/// @var num_buckets the number of buckets
/// @var buckets an array containing all the buckets 
struct hash
{
  int size;
  int num_buckets;
  bucket_t *buckets[];
};

/// @brief the following function is ment to deallocate a string
/// @var object a string 
void destructor_string(obj *object);

/// @brief this function is ment for deallocating an array of strings
/// @var object an array of strings
void destructor_string_array(obj *object);

/// @brief the following function is ment to deallocate a linked list
/// @var object a linked list 
void destructor_linked_list(obj *object);

/// @brief appends a dummy element into a linked list
void linked_list_append();

/// @brief finds out the size of a list
/// @return the size of a linked list
size_t linked_list_size();

/// @brief deallocates a link in a linked list
/// @var c a link
void link_destructor(obj *c);

/// @brief creates a linked list
/// @return a linked list
list_t *list_create();

/// @brief decrements the size of a linked list
void list_negate();

/// @brief increments the size of a linked list
void size_reset();

/// @brief creates a generic hash table
/// @return the hash table
hash_t *demo_hash_table_create();

/// @brief inserts a value into the hash table
/// @var ht a hash table
/// @var key the key we want to insert
/// @var value the value we want to insert
void demo_hash_table_insert(hash_t *ht, int key, char *value);

/// @brief removes an element from the hash table for a given key
/// @var ht a hash table
/// @var key the key for the entry we want to remove
void demo_hash_table_remove(hash_t *ht, int key);

/// @brief finds out the size of the given hash table
/// @var ht the hash table of interest
/// return the size of the hash table
size_t demo_hash_table_size(hash_t *ht);

#endif
