#include <stdlib.h>
#include <stdint.h>
#include "refmem.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "linked_list.h"
#include "../test/lib_for_tests.h"
//#include "common_for_linked_list.h"
#include "../inlupp2/common.h"

list_t *cascade_list = NULL;

size_t counter = 0;

list_t *illegal_alloc_list;

list_t *get_cascade_list()
{
  return cascade_list;
}

void set_cascade_list_to_null()
{
  cascade_list = NULL;
}

int allocs_in_bit_array()
{
  long *bit_array = get_bit_array();
  int counter = 0;
  
  for (long i = 0; i<bit_array_size; i++)
    {

      if (bit_array[i] != 0)
	{ 
	  for (long j = 0; j<row_size; j++)
	    {
	      if (le_bit(bit_array[i], j) != 0)
		{
		  counter += 1;
		}
	    }
	}
    }
  return counter;
}

/*
set_bit(long *A, obj *k)
{
  int i = (long)k%bit_array_size; // i = array index (use: A[i])
  int pos = (long)k%row_size; // pos = bit position in A[i]

  unsigned int flag = 1; // flag = 0000.....00001

  flag = flag << pos; // flag = 0000...010...000   (shifted k positions)

  A[i] = A[i] | flag; // Set the bit at the k-th position in A[i]
}
*/

obj *allocate(size_t bytes, function1_t destructor)
{
  return allocate_array(1, bytes, destructor);
}

obj *allocate_array(size_t elements, size_t bytes, function1_t destructor)
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
      deallocate(ioopm_linked_list_remove(cascade_list,0).value.obj_val);
      alloc = malloc(2*sizeof(uint8_t) + sizeof(function1_t) + elements*bytes);
    }
  if(!alloc) return alloc; //Return NULL if we fail to allocate memory
  
  uint8_t hops = (elements*bytes) / sizeof(void *); //How many pointers our object can hold
  memset(alloc, hops, sizeof(uint8_t));
  
  alloc = (obj *)((char *)alloc + sizeof(uint8_t)); //The location of the byte where refcount is stored
  memset(alloc,0,sizeof(uint8_t)); //Refcount is initially set to 0

  alloc = (obj *)((char *)alloc + sizeof(uint8_t)); //The location where the pointer to the destructor is stored
  memcpy(alloc, &destructor, sizeof(destructor));
  
  alloc = (obj *)((char *)alloc + sizeof(destructor));
  memset(alloc,0,elements*bytes); //Set all bytes to 0, like calloc would do

  obj **pointer_array = get_pointer_array(); 
  if (pointer_array[0] == NULL || (char *)alloc < (char *)(pointer_array[0]))
    {
      pointer_array[0] = alloc;
    }
  if (pointer_array[1] == NULL || (char *)alloc > (char *)(pointer_array[1]))
    {
      pointer_array[1] = alloc;
    }
  
  if (test_bit(get_bit_array(), alloc) != 0)
    {
      //puts("hej");
      alloc = (obj *)((char *)alloc - 2*sizeof(uint8_t) - sizeof(function1_t));
      if (!illegal_alloc_list)
	{
	  illegal_alloc_list = ioopm_linked_list_create(eq_func);
	}
      ioopm_linked_list_append(illegal_alloc_list, (elem_t){.obj_val = alloc});
      return (allocate_array(elements, bytes, destructor));
    }
  else
    {
      set_bit(get_bit_array(), alloc);
    }

  ioopm_linked_list_free_elem(illegal_alloc_list);
  ioopm_linked_list_clear(illegal_alloc_list);
  return alloc;
}

void default_destruct(obj *object)
{
  obj *start = (obj *)((char *)object-sizeof(function1_t)-2*sizeof(uint8_t));
  uint8_t hops = *(uint8_t *)start; //We get how many pointers this object can hold
 
  ///Checks all possible pointer locations in object
  for(uint8_t i = 0; i < hops; i++)
    {
      obj **pointer = (obj **)((char *)object + i*sizeof(obj *));      
      obj *ptr = *(obj **)pointer;

      if (test_bit(get_bit_array(), ptr) != 0)
	{
	  release(ptr);
	}
    }
}

void deallocate_aux(obj *object)
{
  obj *start = (obj *)((char *)object-sizeof(function1_t)-2*sizeof(uint8_t));
  function1_t destructor = *(function1_t *)((obj *)((char *)start+2*sizeof(uint8_t)));

  if (test_bit(get_bit_array(), object) == 0)
    {
      return;
    }

  if(destructor)
    {
      destructor(object);
    }
  else
    {
      default_destruct(object);
    }

  clear_bit(get_bit_array(), object); 
  Free(start);
}

void deallocate(obj *object)
{
  if(!cascade_list) cascade_list = ioopm_linked_list_create(eq_func);

  counter++;
  lib_for_tests_list_negate(); // THIS IS FOR TESTING!
  if(counter == get_cascade_limit() && get_cascade_limit() != 0)
    {
      //If cascade limit is reached, add object to the global cascade list
      ioopm_linked_list_append(cascade_list, (elem_t){.obj_val = object});
      counter = 0;
      return;
    }
  else if(rc(object) == 0)
    {
      deallocate_aux(object);
    }
  counter = 0;
  //  object = NULL;
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
