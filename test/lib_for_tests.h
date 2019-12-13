#pragma once
#include <stdbool.h>
#include "../src/refmem.h"


/**
 * @file lib_for_tests.h
 * @author Elias Insulander, Jonathan Tadese, Robert Paananen, Daniel Westberg, Alex Kangas, Georgios Davakos, Joel Waldenbäck
 * @date 13 December 2019
 * @brief Manual memory management using reference counting.
 *
 * More indepth explanation here of what the header defines
 * 
 * @see http://wrigstad.com/ioopm19/projects/project1.html
 */
typedef struct test_struct test_t;

struct test_struct
{
  char *str;
};

void destructor_string(obj *object);
