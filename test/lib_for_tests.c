#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"

list_t *dereference_list(obj *object)
{
  list_t **ptr = object;
  list_t *list = *ptr;
  return list;
}


void destructor_string(obj *object)
{
  char **ptr = object;
  char *str = *ptr;
  free(str);
}

void destructor_linked_list(obj *object)
{
  list_t *list = dereference_list(object);
  link_t *link = list->head;
  link_t *tmp;
  while (cache != NULL)
    {
      tmp = link->next;
      free(link->str);
      free(link);
      cache = tmp;
    }
}

//------linked list functions-------//

void linked_list_append(obj *object, obj *obj)
{
  list_t *list = dereference_list(object);
  char **pointer = obj;
  char *str = *pointer;

  if(list->head == NULL)
    {
      link_t *link = allocate(sizeof(link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->head = link;
      list->size +=1;
    }
  
  if(list->tail == NULL)
    {
      link_t *link = allocate(sizeof(link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->tail = link;
      list->size +=1;
    }

  if(list->tail != NULL)
    {
      link_t *link = allocate(sizeof(link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->tail->next = link;
      list->tail = link;
      list->size +=1;
    }
}

size_t linked_list_size(obj *object)
{
  list_t *list = dereference_list(object);
  size_t size = list->size;
  return size;
}
