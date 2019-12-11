#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>

typedef struct allocate alloc_t;

struct allocate
{
  uint8_t ref_count;
  obj *object;
  function1_t destructor;
};


obj *allocate(size_t bytes, function1_t destructor)
{
  alloc_t *alloc = malloc(sizeof(alloc_t));
  alloc->ref_count = 0;
  alloc->object = malloc(bytes);
  alloc->destructor = destructor;

  return alloc;
}


obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
{
  alloc_t *alloc = malloc(sizeof(alloc_t));
  alloc->ref_count = 0;
  alloc->object = calloc(elements, bytes);
  alloc->destructor = destructor;

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
  alloc_t *obj = object; //TODO
  alloc_t  obj_real = *obj;
  if(obj_real.ref_count != 0) //Tmp, use rc instead. TODO
    {
      if(has_destructor(obj))
	{
	  obj->destructor(obj->object);	  
	}
      else
	{
	  free(obj->object);	  
	}
      free(obj);
      obj = NULL;
    }
}

