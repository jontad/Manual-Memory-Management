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
typedef struct linked_list list_t;
typedef struct link link_t;

struct string_struct
{
  char *str;
};

struct linked_list
{
  link_t *head;
  link_t *tail;
  size_t size;
};

struct link
{
  link_t *next;
  char *str;
};

/// @brief the following function is ment to deallocate a string
void destructor_string(obj *object);

void destructor_string_array(obj* object);

/// @brief the following function is ment to deallocate a linked list
void destructor_linked_list(obj *object);

void linked_list_append(obj *object, obj * obj);

size_t linked_list_size(obj *object);
