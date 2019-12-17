#include "refmem.h"
#include "linked_list.h"

ioopm_list_t *list = NULL;

ioopm_list_t *create_list()
{
  list = ioopm_linked_list_create();
  return list;
}

ioopm_list_t *linked_list_get()
{
  return list;
}

void cleanup()
{
  link_t *cursor = list->first;
  int counter = 0;
  while(cursor)
    {
      link_t *tmp = cursor->next;
      obj *object = (obj *)cursor->element.obj_val;
      if (rc(object) == 0)
	{
	  deallocate(object); //deallocate_aux instead
	  ioopm_linked_list_remove_link(list,0);
	  counter++;
	}
      cursor = tmp;
    }
}


void shutdown()
{
  ioopm_linked_list_destroy(list);
}
