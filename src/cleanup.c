#include "refmem.h"
#include "linked_list.h"

ioopm_list_t *list = NULL;

void create_list()
{
  list = ioopm_linked_list_create();
}

void cleanup()
{
  link_t *cursor = list->first;
  for (int i = 0; cursor != NULL; i++)
    {
      link_t *tmp = cursor->next;
      obj *object = ioopm_linked_list_get(list, i).value.obj_val;
      if (rc(object) == 0)
	{
	  deallocate(object); //deallocate_aux instead
	}
      cursor = tmp;
    }
}


void shutdown()
{
  deallocate(list);
}
