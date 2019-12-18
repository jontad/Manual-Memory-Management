#pragma once
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

struct string_struct
{
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
  char *str;
};


/// @brief the following function is ment to deallocate a string
void destructor_string(obj *object);

void destructor_string_array(obj* object);

/// @brief the following function is ment to deallocate a linked list
void destructor_linked_list(obj *object);

void linked_list_append();

size_t linked_list_size();

void link_destructor(obj *c);

list_t *list_create();

void list_negate();

void size_reset();
