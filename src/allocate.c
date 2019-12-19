#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "linked_list.h"
#include "../test/lib_for_tests.h"

ioopm_list_t *cascade_list = NULL;

size_t counter = 0;

ioopm_list_t *get_cascade_list()
{
  return cascade_list;
}

void set_cascade_list_to_null()
{
  cascade_list = NULL;
}

obj *allocate(size_t bytes, function1_t destructor)
{
  return allocate_array(bytes, destructor, 1);
}

obj *allocate_array(size_t bytes, function1_t destructor, size_t elements)
{
  //Every time we allocate memory we try to clear up our cascade list
  if(!cascade_list) cascade_list = ioopm_linked_list_create(eq_func);
  if(ioopm_linked_list_size(cascade_list))
    {
      release(ioopm_linked_list_remove(cascade_list,0).value.obj_val);
    }

  //2*sizeof(uint8_t), 1 byte for rc, 1 byte for hops
  obj *alloc = malloc(2*sizeof(uint8_t) + sizeof(function1_t) + elements*bytes);

  //If malloc fails to reserve memory we try to empty our cascade list
  while(alloc == NULL && ioopm_linked_list_size(cascade_list))
    {
      deallocate(ioopm_linked_list_remove_link(cascade_list,0));
      alloc = malloc(2*sizeof(uint8_t) + sizeof(function1_t) + elements*bytes);
    }
  
  uint8_t hops = bytes / sizeof(void *); //How many pointers our object can hold
  memset(alloc, hops, sizeof(uint8_t));
  
  alloc = (obj *)((char *)alloc + sizeof(uint8_t)); //The location of the byte where refcount is stored
  memset(alloc,0,sizeof(uint8_t)); //Refcount is initially set to 0
  alloc = (obj *)((char *)alloc + sizeof(uint8_t)); //The location where the pointer to the destructor is stored
  memcpy(alloc, &destructor, sizeof(destructor));
  
  alloc = (obj *)((char *)alloc + sizeof(destructor));
  memset(alloc,0,elements*bytes); //Set all bytes to 0, like calloc would do

  //Add pointer to the global pointer list
  ioopm_list_t *pointer_list = linked_list_get();
  if (pointer_list) ioopm_linked_list_append(pointer_list, (elem_t){.obj_val = alloc});
  
  return alloc;
}

void remove_from_list(obj *object)
{
  ioopm_list_t *pointer_list = linked_list_get(); 
  if(pointer_list)
    {
      //Get the position of the object pointer in the global pointer list
      int index = ioopm_linked_list_position(pointer_list, (elem_t){.obj_val = object});
      if (index >= 0) ioopm_linked_list_remove_link(pointer_list, index);
    }
}

void default_destruct(obj *object)
{
  obj *start = (obj *)((char *)object-sizeof(function1_t)-2*sizeof(uint8_t));
  uint8_t hops = *(uint8_t *)start; //We get how many pointers this object can hold
  ioopm_list_t *pointer_list = linked_list_get();

  ///Checks all possible pointer locations in object
  for(uint8_t i = 0; i < hops; i++)
    {
      obj **pointer = (obj **)((char *)object + i*sizeof(obj *));      
      obj *ptr = *(obj **)pointer;

      /// if pointer exists in our list we release it
      if(pointer_list && ioopm_linked_list_contains(pointer_list, (elem_t){.obj_val = ptr}))
	{
	  release(ptr);	      
	}
    }
}

void deallocate_aux(obj *object)
{
  obj *start = (obj *)((char *)object-sizeof(function1_t)-2*sizeof(uint8_t));
  function1_t destructor = *(function1_t *)((obj *)((char *)start+2*sizeof(uint8_t)));
  
  if(destructor)
    {
      destructor(object);
    }
  else
    {
      default_destruct(object);
    }
  remove_from_list(object);
  Free(start);
}

void deallocate(obj *object)
{
  if(!cascade_list) cascade_list = ioopm_linked_list_create(eq_func);

  counter++;
  list_negate(); // THIS IS FOR TESTING!
  if(counter == get_cascade_limit() && get_cascade_limit() != 0)
    {
      //If cascade limit is reached, add object to the global cascade list
      ioopm_linked_list_append(cascade_list, (elem_t){.obj_val = object});
      counter = 0;
      return;
    }
  else if(rc(object) == 0) deallocate_aux(object);
  counter = 0;
}

void retain(obj *object)
{
  if(object)
    {
      obj *tmp = (obj *)((char *)object-sizeof(function1_t)-sizeof(uint8_t));
      uint8_t ref_count = *(uint8_t *)tmp;
      if(ref_count != 255) ref_count++;
      memset(tmp,ref_count,1);
    }
}

void release(obj *object)
{
  if(object)
    {
      obj *tmp = (obj *)((char *)object-sizeof(function1_t)-sizeof(uint8_t));
      uint8_t ref_count = *(uint8_t *)tmp;
      if(ref_count != 0) ref_count--;
      memset(tmp,ref_count,1);
      if(ref_count == 0) deallocate(object);
    }
}

size_t rc(obj *object)
{
  obj *tmp = (obj *)((char *)object-sizeof(function1_t)-sizeof(uint8_t));
  uint8_t ref_count = *(uint8_t *)tmp;
  return ref_count;
}
