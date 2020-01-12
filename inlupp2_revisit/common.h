#ifndef _COMMONH_
#define _COMMONH_

#include <stdlib.h>
#include <stdbool.h>



/**
 * @file common.h
 * @author Emanuel Jansson & Jonathan Tadese
 * @date 1 Nov 2019
 * @brief Contains miscellanous typedefinitions
 */
//common and slam


/// @brief contains pointer to first and last link. Also contains size of list, and an equality function to compare elements in list
typedef struct list ioopm_list_t;

/// @brief contains pointer to next link and a value of type elem_t
typedef struct link ioopm_link_t;

/// @brief type of function that compares two values and returns bool
/// @param element argument that can be compared
/// @param extra extra argument for function
typedef bool(*ioopm_char_predicate)(ioopm_link_t *element, void *extra);

/// @brief type of function to be applied to hash table
/// @param element arguemmt that can be used
/// @param extra extra argument for function
typedef void(*ioopm_apply_char_function)(ioopm_link_t **element, void *extra);


/// @brief contains name, desc, price, available amount and location of merchandise 
typedef struct merch merch_t;

/// @brief contains name, amount, price per unit and a pointer to corresponding merchandise 
typedef struct item item_t;

/// @brief contains id of cart and pointer to list of items 
typedef struct cart cart_t;

/// @brief contains shelf name and amount on shelf
typedef struct shelf shelf_t;


/// @brief general type to store different types in hash tables or linked lists
typedef union elem elem_t;

/// @brief union of different types to be stored in hash tables or linked lists
union elem
{
  int ioopm_int; 
  unsigned int ioopm_u_int; 
  bool ioopm_bool;
  char *ioopm_str;
  float ioopm_float;
  void *ioopm_void_ptr;
  merch_t *ioopm_merch;
  shelf_t *ioopm_shelf;
  cart_t *ioopm_cart; 
  item_t *ioopm_item; 
};



/// @brief entries in a hash table
typedef struct entry entry_t;

/// @brief contains information to build hash table
typedef struct hash_table ioopm_hash_table_t;

/// @brief returns boolean, and if true returns value
typedef struct option option_t;

/// @brief hashes key
/// @param elem key that will be hashed 
typedef unsigned long(*hash_function)(elem_t key);

/// @brief type of function that compares two elements and returns bool whether they are equal.
/// @param a first element that will be compared
/// @param b second element that will be compared
typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);  

/// @brief type of function that compares two values and returns bool
/// @param key argument that can be compared
/// @param value argument that can be compared
/// @param extra extra argument for function
typedef bool(*ioopm_predicate)(elem_t key, elem_t value, void *extra);

/// @brief type of function to be applied to hash table
/// @param key arguemmt that can be used
/// @param value argument that can be used
/// @param extra extra argument for function
typedef void(*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);

/// @brief contains linked list and pointer to next and current location
typedef struct iter ioopm_list_iterator_t;

/// @brief contains hash tables of merchandise, shelves and carts. Also contains counter for id to be used for carts. 
typedef struct database ioopm_database_t;


#endif


