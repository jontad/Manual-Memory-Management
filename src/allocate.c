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
  if(!cascade_list) cascade_list = ioopm_linked_list_create(eq_func);
  if(ioopm_linked_list_size(cascade_list))
    {
      deallocate(ioopm_linked_list_remove(cascade_list,0).value.obj_val);
    }
  
  obj *alloc = malloc(sizeof(uint8_t) + sizeof(function1_t) + bytes);

  while(alloc == NULL && ioopm_linked_list_size(cascade_list))
    {
      deallocate(ioopm_linked_list_remove_link(cascade_list,0));
      alloc = malloc(sizeof(uint8_t) + sizeof(function1_t) + bytes);
    }

  memset(alloc,0,1);
  memcpy((obj *)((char *)alloc+sizeof(uint8_t)),&destructor,sizeof(destructor));
  alloc = (obj *)((char *)alloc + sizeof(destructor) + sizeof(uint8_t));
  
  ioopm_list_t *list = linked_list_get();
  if (list) ioopm_linked_list_append(list, (elem_t){.obj_val = alloc});
  return alloc;

}


obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
{

  if(!cascade_list) cascade_list = ioopm_linked_list_create(eq_func);
  if(ioopm_linked_list_size(cascade_list))
    {
      deallocate(ioopm_linked_list_remove(cascade_list,0).value.obj_val);
    }
  
  obj *alloc = malloc(sizeof(uint8_t) + sizeof(function1_t) + elements*bytes);

  while(alloc == NULL && ioopm_linked_list_size(cascade_list))
    {
      deallocate(ioopm_linked_list_remove_link(cascade_list,0));
      alloc = malloc(sizeof(uint8_t) + sizeof(function1_t) + elements*bytes);
    }

  memset(alloc,0,1);
  memcpy((obj *)((char *)alloc+sizeof(uint8_t)),&destructor,sizeof(destructor));
  alloc = (obj *)((char *)alloc + sizeof(destructor) + sizeof(uint8_t));
  memset(alloc,0,elements*bytes);
  
  ioopm_list_t *list = linked_list_get();
  if (list) ioopm_linked_list_append(list, (elem_t){.obj_val = alloc});
  
  return alloc;
}

void deallocate_aux(obj *object)
{
  obj *tmp = (obj *)((char *)object-sizeof(function1_t)-sizeof(uint8_t));
  function1_t destructor = *(function1_t *)((obj *)((char *)tmp+sizeof(uint8_t)));
  
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

size_t counter = 0;

void deallocate(obj *object)
{
  if(!cascade_list) cascade_list = ioopm_linked_list_create(eq_func);

  counter++;
  list_negate(); // THIS IS FOR TESTING!
  if(counter == get_cascade_limit())
    {
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
      ref_count++;
      memset(tmp,ref_count,1);
    }
}

void release(obj *object)
{
  if(object)
    {
      obj *tmp = (obj *)((char *)object-sizeof(function1_t)-sizeof(uint8_t));;
      uint8_t ref_count = *(uint8_t *)tmp;
      ref_count--;
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
