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

/****************** MARCROS ***********************/


//Free
#define Free(ptr) {free(ptr); ptr = NULL;} 

//Option
#define Success(v)      (option_t) { .success = true, .value = v };
#define Failure()       (option_t) { .success = false };
#define Successful(o)   (o.success == true)
#define Unsuccessful(o) (o.success == false)

//Elements
#define int_elem(i) (elem_t) {.int1 = (i)}
#define unsigned_elem(u) (elem_t) {.u_int = (u)}
#define bool_elem(b) (elem_t) {.boolean = (b)}
#define str_elem(s) (elem_t) {.str = (s)}
#define float_elem(f) (elem_t) {.float1 = (f)}
#define void_elem(v) (elem_t) {.void_ptr = (v)}
#define merch_elem(m) (elem_t) {.merch = (m)}
#define shelf_elem(sh) (elem_t) {.shelf = (sh)}
#define cart_elem(c) (elem_t) {.cart = (c)}
#define item_elem(ci) (elem_t) {.item = (ci)}

//Reference count
#define obj_elem(o) (elem_t) { .obj_val=(o) }

/****************** COMMON TYPE DEFINTIONS ***********************/


/// @brief 
typedef void obj;

/// @brief
/// @param
typedef void(*function1_t)(obj *);


/// @brief contains pointer to first and last link. Also contains size of list, and an equality function to compare elements in list
typedef struct list list_t;

/// @brief contains pointer to next link and a value of type elem_t
typedef struct link link_t;

/// @brief type of function that compares two values and returns bool
/// @param element argument that can be compared
/// @param extra extra argument for function
typedef bool(*char_predicate)(link_t *element, void *extra);

/// @brief type of function to be applied to hash table
/// @param element arguemmt that can be used
/// @param extra extra argument for function
typedef void(*apply_char_function)(link_t **element, void *extra);


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
  int int1;  
  unsigned int u_int; 
  bool boolean;
  char *str;
  float float1;
  void *void_ptr;
  merch_t *merch;
  shelf_t *shelf;
  cart_t *cart; 
  item_t *item;
  obj *obj_val;
};


/// @brief entries in a hash table
typedef struct entry entry_t;

/// @brief contains information to build hash table
typedef struct hash_table hash_table_t;

/// @brief returns boolean, and if true returns value
typedef struct option option_t;

/// @brief hashes key
/// @param elem key that will be hashed 
typedef unsigned long(*hash_function)(elem_t key);

/// @brief type of function that compares two elements and returns bool whether they are equal.
/// @param a first element that will be compared
/// @param b second element that will be compared
typedef bool(*eq_function)(elem_t a, elem_t b);  

/// @brief type of function that compares two values and returns bool
/// @param key argument that can be compared
/// @param value argument that can be compared
/// @param extra extra argument for function
typedef bool(*predicate)(elem_t key, elem_t value, void *extra);

/// @brief type of function to be applied to hash table
/// @param key arguemmt that can be used
/// @param value argument that can be used
/// @param extra extra argument for function
typedef void(*apply_function)(elem_t key, elem_t *value, void *extra);

/// @brief contains linked list and pointer to next and current location
typedef struct iter list_iterator_t;

/// @brief contains hash tables of merchandise, shelves and carts. Also contains counter for id to be used for carts. 
typedef struct database database_t;

/******************** COMMON STRUCTS ************************/

struct option
{
  bool success;
  elem_t value; 
};

/******* HASH TABLE *******/

struct hash_table
{
  float load_factor;
  size_t capacity;
  entry_t **buckets;
  hash_function hash_func;
  eq_function key_eq_func;
  eq_function value_eq_func;
  size_t size;
};

/******* LINKED LIST *******/

struct link
{
  elem_t value;
  link_t *next;
};

struct list
{
  size_t list_size; 
  eq_function equal;
  link_t *first;
  link_t *last;
};

/******* FRONTEND & BACKEND *******/

struct database
{
  hash_table_t *merch_ht;
  hash_table_t *shelves_ht;
  hash_table_t *carts;
  unsigned int id_counter; 
};

struct item
{
  char *name;
  unsigned int amount; // amount of merch in cart
  unsigned int price_per_unit;
  merch_t *pointer; //pointer to merch in ht
};

struct cart
{
  unsigned int id; // key 
  list_t *basket;                                     
};

struct shelf
{
  char *shelf_name;
  unsigned int amount; //amount on shelf
};

struct merch
{
  char *name; //key
  char *desc;
  unsigned int price_per_unit;
  unsigned int available_amount; //amount of merch in stock that is not in cart
  list_t *stock;
};



bool eq_func(elem_t a, elem_t b);
list_t *get_cascade_list();
void set_cascade_list_to_null();

#endif


