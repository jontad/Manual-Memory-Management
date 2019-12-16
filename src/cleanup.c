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
  for (int i = 0; cursor != NULL; i++)
    {
      link_t *tmp = cursor->next;
      link_t *object = ioopm_linked_list_remove_link(list, i);
      if (rc(object) == 0)
	{
	  i--;
	  deallocate(object); //deallocate_aux instead
	}
      cursor = tmp;
    }
}


void shutdown()
{
  deallocate(list);
}
