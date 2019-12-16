#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../test/lib_for_tests.h"

typedef struct allocate alloc_t;

struct allocate
{
  uint8_t ref_count;
  function1_t destructor;
};

struct deallocate_queue
{
  size_t amount;
  list_t *list;
};


struct deallocate_queue init()
{
  struct deallocate_queue deallocate_queue = { .amount = 0, .list = linked_list_create() };
  return deallocate_queue;
}
//struct deallocate_queue dealloqate_queue = init();
//struct deallocate_queue init();

struct deallocate_queue deallocate_queue = { .amount = 100, .list = NULL };


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

//char *str = ((list_t *)object)->head->elem;
//puts(str);

void deallocate(obj *object)
{
  if (deallocate_queue.list == NULL) { deallocate_queue.list = linked_list_create(); }

  linked_list_append(deallocate_queue.list, object);
  while (deallocate_queue.list->tail)
    {
      //Ta tail så att det inte blir samma varje gång...
      obj *tmp = deallocate_queue.list->tail->elem;
      tmp = tmp-sizeof(function1_t);
      function1_t destructor = *(function1_t *)(tmp);
      if(destructor)
	{
	  destructor(deallocate_queue.list->tail->elem);
	}
      if (deallocate_queue.amount == 0)
	{
	  return;
	}
      free(tmp-1);
    }
}
//Måste lägga till så att element tas bort från deallocate_queue.list också!!!

/*
void deallocate(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  uint8_t ref_count = *(uint8_t *)tmp;
  function1_t destructor = *(function1_t *)(tmp+1);
  if(destructor)
	{
	  destructor(object);	  
	}
  Free(tmp);
}
*/

void retain(obj *object)
{
  if(object)
    {
      obj *tmp = object-sizeof(function1_t)-1;
      uint8_t ref_count = *(uint8_t *)tmp;
      ref_count++;
      memset(tmp,ref_count,1);
    }
}

void release(obj *object)
{
  if(object)
    {
      obj *tmp = object-sizeof(function1_t)-1;
      uint8_t ref_count = *(uint8_t *)tmp;
      ref_count--;
      memset(tmp,ref_count,1);
      if(ref_count == 0) deallocate(object);
    }
}

size_t rc(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  uint8_t ref_count = *(uint8_t *)tmp;
  return ref_count;
}
