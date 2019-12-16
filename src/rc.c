#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**
 * @file rc.c
 * @author Elias Insulander & Alex Kangas
 * @date 16 Dec 2019
 * @brief Gets reference count of an object, adds and subtracts to it.
 */

typedef struct allocate alloc_t;

struct allocate
{
  uint8_t ref_count;
  function1_t destructor;
};



/// @brief adds 1 to reference count of an object
/// @param object object to increase reference count of
void rc_add_one(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  *(uint8_t *)tmp = *(uint8_t *)tmp + 1;
}

/// @brief subtracts 1 from reference count of an object
/// @param object object to decrease reference count of
void rc_sub_one(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  *(uint8_t *)tmp = *(uint8_t *)tmp - 1;
}

/// @brief adds 1 to reference count if object exists
/// @param object object to increase reference count of
void retain(obj *object)
{
  if (object == NULL)
    {
      return;
    }
  else
    {
      rc_add_one(object);
    }
}

/// @brief subtracts 1 to reference count if object exists, destroys object if reference count is 1
/// @param object object to decrease reference count of
void release(obj *object)
{
  if (object == NULL)
    {
      return;
    }
  else if (rc(object) == 1)
    {
      rc_sub_one(object);
      //deallocate(object);
    }
  else
    {
      rc_sub_one(object);
    }
}

/// @brief gets reference count of an object
/// @param object object to get reference count of
/// @return reference count
size_t rc(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  uint8_t ref_count = *(uint8_t *)tmp;

  return ref_count;
}
  
