#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"


void destructor_string(obj *object)
{
  char **ptr = object;
  char *str = *ptr;
  free(str);
}

void destructor_linked_list(obj *object)
{
  list_t **ptr = object;
  list_t *list = *ptr;
  link_t *link = list->head;
  link_t *tmp;
  while (link != NULL)
    {
      tmp = link->next;
      free(link->str);
      free(link);
      link = tmp;
    }
}

//------linked list functions-------//

void linked_list_append(obj *object, obj *obj)
{
  list_t **ptr = object;
  list_t *list = *ptr;
  char **pointer = obj;
  char *str = *pointer;

  if(list->tail != NULL)
    {
      link_t *link = allocate(sizeof(link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->tail->next = link;
      list->tail = link;
      list->size +=1;
    }
    
  if(list->head == NULL)
    {
      link_t *link = allocate(sizeof(link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->head = link;
    }
  
  if(list->tail == NULL)
    {
      link_t *link = allocate(sizeof(link_t), NULL);
      link->next = NULL;
      link->str = str;
      list->tail = link;
      list->size +=1;
    }
}

size_t linked_list_size(obj *object)
{
  list_t **ptr = object;
  list_t *list = *ptr;
  size_t size = list->size;
  return size;
}
