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


obj *allocate(size_t bytes, function1_t destructor)
{
  /*alloc_t *info = malloc(sizeof(alloc_t));
  info->ref_count = 0;
  info->destructor = destructor;
  */
  obj *alloc = malloc(1 + sizeof(function1_t) + bytes);
  memset(alloc,0,1);
  memcpy(alloc+1,&destructor,sizeof(destructor));
  alloc += sizeof(destructor) + 1;
  return alloc;

}


obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
{
  obj *alloc = malloc(1 + sizeof(function1_t) + elements*bytes);
  memset(alloc,0,1);
  memcpy(alloc+1,&destructor,sizeof(destructor));
  alloc += sizeof(destructor) + 1;
  return alloc;
}


bool has_destructor(alloc_t *obj){
  return obj->destructor;
}

//Might not be needed!
void set_destructor(obj *object, function1_t destructor)
{
  alloc_t *obj = object;

  obj->destructor = destructor;
}

void deallocate(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  uint8_t ref_count = *(uint8_t *)tmp;
  function1_t destructor = *(function1_t *)(tmp+1);
  if((ref_count == 0))
    {
      if(destructor)
	{
	  destructor(object);	  
	}
      Free(tmp);
    }
}

