#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../test/lib_for_tests.h"
#include <assert.h>
#include "linked_list.h"
  
struct deallocate_queue
{
  size_t amount;
  list_t *list;
};
  
struct deallocate_queue deallocate_queue = { .amount = 100, .list = NULL };



obj *allocate(size_t bytes, function1_t destructor)
{
  obj *alloc = malloc(1 + sizeof(function1_t) + bytes);
  memset(alloc,0,1);
  memcpy((obj *)((char *)alloc+1),&destructor,sizeof(destructor));
  alloc = (obj *)((char *)alloc + sizeof(destructor) + 1);
  ioopm_list_t *list = linked_list_get();
  if (list) ioopm_linked_list_append(list, (elem_t){.obj_val = alloc});
  return alloc;
}

obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
{
  obj *alloc = malloc(1 + sizeof(function1_t) + elements*bytes);
  memset(alloc,0,1);
  memcpy((obj *)((char *)alloc+1),&destructor,sizeof(destructor));
  alloc = (obj *)((char *)alloc + sizeof(destructor) + 1);
  memset(alloc,0,elements*bytes);
  
  ioopm_list_t *list = linked_list_get();
  if (list) ioopm_linked_list_append(list, (elem_t){.obj_val = alloc});
  
  return alloc;
}

void deallocate_aux(obj *object)
{
  obj *tmp = (obj *)((char *)object-sizeof(function1_t)-1);
  function1_t destructor = *(function1_t *)((obj *)((char *)tmp+1));
  
  if(destructor)
    {
      destructor(object);
    }
  
  ioopm_list_t *list = linked_list_get();
  if(list)
    {
      int index = ioopm_linked_list_position(list, (elem_t){.obj_val = object});
      if (index >= 0) ioopm_linked_list_remove_link(list, index);
    }
  
  Free(tmp);
}

/*
void deallocate(obj *object)
{
  while (!is_empty(stack))
      {
	obj *tmp = pop(stack);
	function1_t destructor = *(function1_t *)(tmp-sizeof(function1_t));
	if (deallocate_queue.amount > 0 && destructor)
	  {
	    destructor(tmp);
	  }
	if (deallocate_queue.amount == 0)
	  {
	    return;
	  }
	free(tmp-sizeof(function1_t)-sizeof(uint8_t));
	linked_list_remove(deallocate_queue.list, 0);
      }
}
*/

/*
void deallocate(obj *object)
{
  if (deallocate_queue.list == NULL) { deallocate_queue.list = linked_list_create(); }

  //linked_list_append(deallocate_queue.list, object);
  while (deallocate_queue.list->tail)
    {
      //tail->elem så att det inte blir en loop... Betyder att det blir LIFO prioritet på denna funktion
      obj *tmp = deallocate_queue.list->tail->elem;
      function1_t destructor = *(function1_t *)(tmp-sizeof(function1_t));
      if (destructor)
	{
	  destructor(tmp);
	}
      if (deallocate_queue.amount == 0)
	{
	  return;
	}
      free(tmp-sizeof(function1_t)-sizeof(uint8_t));
    }
}
//Måste lägga till så att element tas bort från deallocate_queue.list också!!!
*/

void deallocate(obj *object)
{
  if(rc(object) == 0) deallocate_aux(object);
}

void retain(obj *object)
{
  if(object)
    {
      obj *tmp = (obj *)((char *)object-sizeof(function1_t)-1);
      uint8_t ref_count = *(uint8_t *)tmp;
      ref_count++;
      memset(tmp,ref_count,1);
    }
}

void release(obj *object)
{
  if(object)
    {
      obj *tmp = (obj *)((char *)object-sizeof(function1_t)-1);;
      uint8_t ref_count = *(uint8_t *)tmp;
      ref_count--;
      memset(tmp,ref_count,1);
      if(ref_count == 0) deallocate(object);
    }
}

size_t rc(obj *object)
{
  obj *tmp = (obj *)((char *)object-sizeof(function1_t)-1);
  uint8_t ref_count = *(uint8_t *)tmp;
  return ref_count;
}
