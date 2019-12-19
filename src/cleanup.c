#include "refmem.h"
#include "linked_list.h"

ioopm_list_t *list = NULL;

bool eq_func(elem_t a, elem_t b)
{
  obj *a_ptr = a.obj_val;
  obj *b_ptr = b.obj_val;
  return a_ptr == b_ptr;
}

ioopm_list_t *create_list()
{
  list = ioopm_linked_list_create(eq_func);
  return list;
}

ioopm_list_t *linked_list_get()
{
  if(list) return list;
  else return create_list();
}

void cleanup()
{
  if(list)
    {
      link_t *cursor = list->first;
      int counter = 0;
      while(cursor)
	{
	  link_t *tmp = cursor->next;
	  obj *object = (obj *)cursor->element.obj_val;
	  if (rc(object) == 0)
	    {
	      deallocate_aux(object);
	    }
	  else counter++;
	  cursor = tmp;
	}
    }
}

void shutdown()
{
  while(ioopm_linked_list_size(list))
    {
      obj *object = ioopm_linked_list_get(list,0).value.obj_val;
      deallocate_aux(object);
    }
  ioopm_list_t *cas_list = get_cascade_list();
  if(cas_list) ioopm_linked_list_destroy(cas_list);
  ioopm_linked_list_destroy(list);
  list = NULL;
  set_cascade_list_to_null();
}
