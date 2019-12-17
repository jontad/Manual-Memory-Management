#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "linked_list.h"


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
  ioopm_list_t *list = linked_list_get();
  if (list) ioopm_linked_list_append(list, (elem_t){.obj_val = alloc});
  return alloc;

}


obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
{
  obj *alloc = malloc(1 + sizeof(function1_t) + elements*bytes);
  memset(alloc,0,1);
  memcpy(alloc+1,&destructor,sizeof(destructor));
  alloc += sizeof(destructor) + 1;
  memset(alloc,0,elements*bytes);
  ioopm_list_t *list = linked_list_get();
  if (list) ioopm_linked_list_append(list, (elem_t){.obj_val = alloc});
  return alloc;
}

void deallocate_aux(obj *object)
{
  obj *tmp = object-sizeof(function1_t)-1;
  function1_t destructor = *(function1_t *)(tmp+1); 
  if(destructor)
    {
      destructor(object);
    }
  Free(tmp);
}
  
void deallocate(obj *object)
{
  if(rc(object) == 0) deallocate_aux(object);
}

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
