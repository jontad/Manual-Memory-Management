#include "refmem.h"
#include "linked_list.h"

list_t *pointer_list = NULL;

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
