#include "refmem.h"
#include "linked_list.h"
#include <stdint.h>

obj *pointer_array[] = {NULL, NULL};
long bit_array[bit_array_size];

long *get_bit_array()
{
  return bit_array;
}

obj **get_pointer_array()
{
  return pointer_array;
}

bool eq_func(elem_t a, elem_t b)
{
  obj *a_ptr = a.obj_val;
  obj *b_ptr = b.obj_val;
  return a_ptr == b_ptr;
}

void cleanup()
{
  obj **pointer_array = get_pointer_array();
  obj *low = pointer_array[0];
  obj *high = pointer_array[1];

  long i = (long)low;
  while (i <= (long)high)
    {
      if (test_bit(get_bit_array(), i) != 0)
	{
	  if (rc((obj *)i) == 0)
	    {
	      deallocate((obj *)i);
	    }
	}
      i = i + sizeof(void *);
    }
}

void shutdown()
{
  obj **pointer_array = get_pointer_array();
  obj *low = pointer_array[0];
  obj *high = pointer_array[1];
  //printf("%d\n", (long)low);
  //printf("%d\n", (long)high);

  long i = (long)low;
  while (i <= (long)high)
    {
      if (test_bit(get_bit_array(), i) != 0)
	{
	  deallocate_aux((obj *)i); 
	}
      i = i + sizeof(void *);
    }
  
  list_t *cascade_list = get_cascade_list();
  if(cascade_list)
    {
      ioopm_linked_list_destroy(cascade_list);
      set_cascade_list_to_null();
    }

  pointer_array[0] = NULL;
  pointer_array[1] = NULL;
}
