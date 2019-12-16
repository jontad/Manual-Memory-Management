#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <stdio.h>


void destructor_string(obj *object)
{
  char **ptr = object;
  char *str = *ptr;
  free(str);
}

void destructor_string_array(obj *object)
{
  char **array_ptr = object;
  for(int i = 0; i < 10; i++)
    {
      free(array_ptr[i]);
    }
}

void destructor_linked_list(obj *object)
{
  list_t *list = object;
  new_link_t *link = list->head;
  new_link_t *tmp;
  while (link != NULL)
    {
      tmp = link->next;
      free(link->str);
      deallocate(link);
      link = tmp;
    }
}

//------linked list functions-------//

void linked_list_append(obj *object, obj *obj)
{
  list_t *list = object;
  char *str = ((char*) obj);

  if(list->tail != NULL)
    {
      new_link_t *link = allocate(sizeof(new_link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->tail->next = link;
      list->tail = link;
    }
  else
     {
      new_link_t *link = allocate(sizeof(new_link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->head = link;
      list->tail = link;
     }
  
      list->size +=1;
}

size_t linked_list_size(obj *object)
{
  list_t *list = object;
  size_t size = list->size;
  return size;
}
