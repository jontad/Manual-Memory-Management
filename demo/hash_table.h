#ifndef _HASH_TABLEH_
#define _HASH_TABLEH_

/**
 * @file hash_table.h
 * @author Emanuel Jansson & Elias Insulander
 * @date 1 Sep 2019
 * @brief Hash table that maps keys to values.
 */

#include <stdlib.h>
#include <stdbool.h>
#include "inlupp_linked_list.h"


/// @brief Create a new hash table
/// @return A new empty hash table
/// @param hash_func hash function for appropriate key datatype
/// @param key_eq key_equivalent function for appropriate key datatype
/// @param value_eq value_equivalent function for appropriate value datatype
/// @param load_factor maximum load-factor before resizing
/// @param capacity Initial number of buckets
/// @return A new empty hash table
hash_table_t *hash_table_create(hash_function hash_func, eq_function key_eq, eq_function value_eq, float load_factor, size_t capacity);

/// @brief Delete a hash table and free its memory
/// param ht a hash table to be deleted
void hash_table_destroy(hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void hash_table_insert(hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return true and the value mapped to by key if successful, otherwise false
option_t hash_table_lookup(hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key, or "Non existent key" if key doesn't exist
elem_t hash_table_remove(hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table
/// @param ht hash table operated upon
/// @return the number of key => value entries in the hash table
size_t hash_table_size(hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param ht hash table operated upon
/// @return true if size == 0, else false
bool hash_table_is_empty(hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param ht hash table operated upon
void hash_table_clear(hash_table_t *ht);

/// @brief return the keys for all entries in a linked list (in same order as hash_table_values)
/// @param ht hash table operated upon
/// @return a linked list of keys for hash table ht
list_t *hash_table_keys(hash_table_t *ht);

/// @brief return the values for all entries in a hash map (in no particular order, but same as hash_table_keys)
/// @param ht hash table operated upon
/// @return a linked list of values for hash table ht
list_t *hash_table_values(hash_table_t *ht);

/// @brief check if a hash table has an entry with a given key
/// @param ht hash table operated upon
/// @param key the key sought
bool hash_table_has_key(hash_table_t *ht, elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param ht hash table operated upon
/// @param value the value sought
bool hash_table_has_value(hash_table_t *ht, elem_t value);


/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool hash_table_all(hash_table_t *ht, predicate pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool hash_table_any(hash_table_t *ht, predicate pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param ht hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void hash_table_apply_to_all(hash_table_t *ht, apply_function apply_fun, void *arg);




/// @brief apply a hash function to a key
/// @param key key operated upon
/// @return An unsigned int value based on element
unsigned long int_hash_func(elem_t key);


/// @brief apply a hash function to a key
/// @param uns_int uns_int operated upon
/// @return An unsigned int value based on element
unsigned long uns_int_hash_func(elem_t uns_int);

/// @brief apply a hash function to a key
/// @param boolean boolean operated upon
/// @return An unsigned int value based on element
unsigned long bool_hash_func(elem_t boolean);

/// @brief apply a hash function to a key
/// @param float_num float_num operated upon
/// @return An unsigned int value based on element
unsigned long float_hash_func(elem_t float_num);

/// @brief apply a hash function to a key
/// @param string string operated upon
/// @return An unsigned int value based on element
unsigned long str_hash_func(elem_t string);

/// @brief apply a hash function to a key
/// @param pointer pointer operated upon
/// @return An unsigned int value based on element
unsigned long pointer_hash_func(elem_t pointer);

/// @brief apply a hash function to a key
/// @param merch merch operated upon
/// @return An unsigned int value based on element
unsigned long merch_hash_func(elem_t merch);


/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_int(elem_t a, elem_t b);

/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_uns_int(elem_t a, elem_t b);

/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_bool(elem_t a, elem_t b);

/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_float(elem_t a, elem_t b);

/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_str(elem_t a, elem_t b);

/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_pointer(elem_t a, elem_t b);

/// @brief check if a and b are equal
/// @param a elem_t to compare
/// @param b elem_t to compare
/// @return true if a and b are equal, false if not
bool equality_function_merch(elem_t a, elem_t b);
#endif
