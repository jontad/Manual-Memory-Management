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

struct string_struct
{
  char *str;
};

/// @brief the following function is ment to deallocate a string
void destructor_string(obj *object);
