#include "refmem.h"
#include "linked_list.h"
#include <stdint.h>


#define bit_array_size 32
#define set_bit(A,k)     ( A[(k/bit_array_size)] |= (1 << (k%32)) )
#define clear_bit(A,k)   ( A[(k/bit_array_size)] &= ~(1 << (k%32)) )
#define test_bit(A,k)    ( A[(k/bit_array_size)] & (1 << (k%32)) )
int bit_array[bit_array_size];

list_t *pointer_list;

int* get_bit_array()
{
  return bit_array;
}

bool eq_func(elem_t a, elem_t b)
{
  obj *a_ptr = a.obj_val;
  obj *b_ptr = b.obj_val;
  return a_ptr == b_ptr;
}

list_t *create_pointer_list()
{
  pointer_list = ioopm_linked_list_create(eq_func);
  return pointer_list;
}

list_t *linked_list_get_list()
{
  if(pointer_list) return pointer_list;
  else return create_pointer_list();
}

void cleanup()
{
  if(pointer_list)
    {
      link_t *cursor = pointer_list->first;
      while(cursor)
	{
	  link_t *tmp = cursor->next;
	  obj *object = (obj *)cursor->value.obj_val;
	  if (rc(object) == 0)
	    {
	      deallocate_aux(object);
	    }
	  cursor = tmp;
	}
    }
  return;
}

void shutdown()
{
  while(ioopm_linked_list_size(pointer_list))
    {
      option_t object = ioopm_linked_list_get(pointer_list,0);
      if(Successful(object)) deallocate_aux(object.value.obj_val);
    }
  
  list_t *cascade_list = get_cascade_list();
  if(cascade_list)
    {
      ioopm_linked_list_destroy(cascade_list);
      set_cascade_list_to_null();
    }
  
  ioopm_linked_list_destroy(pointer_list);
  pointer_list = NULL;
}




/****************** BITARRAY STUFF *******************/


obj *get_object_from_bit(uint8_t position)
{
  if (pointer_list)
    {
      link_t *cursor = pointer_list->first;
      while(cursor)
	{
	  obj *object = (obj *)cursor->value.obj_val;
	  obj *tmp = (obj *)((char *)object-sizeof(function1_t)-sizeof(uint8_t));
	  uint8_t bit_pos = *(uint8_t *)tmp;
	  if (test_bit(bit_array, bit_pos))
	    {
	      return object;
	    }
	  cursor = cursor->next;
	}
    }
  return NULL;
}


void cleanup_with_bitarray()
{
  for (int i = 0; i<bit_array_size; i++)
    {
      if (!bit_array[i]) continue;

      for (int j = 0; j<32; j++)
	{
	  if (!test_bit(bit_array, j)) continue;

	  uint8_t bit_position = bit_array_size * i + j;
	  obj *object = get_object_from_bit(bit_position);
	  if (rc_with_bitarray(object) == 0)
	    {
	      deallocate_aux_with_bitarray(object);
	    }
	}
    }
}

void shutdown_with_bitarray()
{
  for (int i = 0; i<bit_array_size; i++)
    {
      if (!bit_array[i]) continue;

      for (int j = 0; j<32; j++)
	{
	  if (!test_bit(bit_array, j)) continue;

	  uint8_t bit_position = bit_array_size * i + j;
	  obj *object = get_object_from_bit(bit_position);
	  deallocate_aux_with_bitarray(object);
	}
    }
  
  list_t *cascade_list = get_cascade_list();
  if(cascade_list)
    {
      ioopm_linked_list_destroy(cascade_list);
      set_cascade_list_to_null();
    }
  
  ioopm_linked_list_destroy(pointer_list);
  pointer_list = NULL;
}
