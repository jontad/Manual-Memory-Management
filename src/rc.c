#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct allocate alloc_t;

struct allocate
{
  uint8_t ref_count;
  function1_t destructor;
};


void rc_add_one(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  *(uint8_t *)tmp = *(uint8_t *)tmp + 1;
}

void rc_sub_one(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  *(uint8_t *)tmp = *(uint8_t *)tmp - 1;
}


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


size_t rc(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  uint8_t ref_count = *(uint8_t *)tmp;

  return ref_count;
}
  
