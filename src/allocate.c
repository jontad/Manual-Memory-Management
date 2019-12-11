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
  alloc_t *info = malloc(sizeof(alloc_t));
  info->ref_count = 0;
  info->destructor = destructor;

  obj *alloc = malloc(sizeof(alloc_t) + bytes);
  memcpy(alloc,info,sizeof(*info));
  alloc += sizeof(alloc_t);
  free(info);
  return alloc;

}


obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
{
  alloc_t *info = malloc(sizeof(alloc_t));
  info->ref_count = 0;
  info->destructor = destructor;

  obj *alloc = malloc(sizeof(alloc_t) + elements*bytes);
  memcpy(alloc,info,sizeof(*info));
  alloc += sizeof(alloc_t);
  free(info);
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
  alloc_t *info = malloc(sizeof(alloc_t));
  obj *tmp = object-sizeof(alloc_t);
  memcpy(info,tmp,sizeof(alloc_t));
  uint8_t ref_count = info->ref_count;
  function1_t destructor = info->destructor;

  if((ref_count == 0)) //Tmp, use rc instead. TODO
    {
      if(destructor)
	{
	  destructor(object);	  
	}
      Free(tmp);	  
      Free(info);
    }
}

