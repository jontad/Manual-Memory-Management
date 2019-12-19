#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <stdio.h>

list_t *list = NULL;

size_t list_size = 0;

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
  release(link);
}

void link_destructor(obj *link)
{
  release(((new_link_t *) link)->next);
}

void list_negate()
{
  list_size--;
}

void size_reset()
{
  list_size = 0;
}

list_t *list_create()
{
  list = allocate(sizeof(list_t), destructor_linked_list);
  list->head = list->tail = NULL;
  list->size = 0;
  return list;
}

void linked_list_append()
{
  if(list->tail != NULL)
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      release(list->tail);
      list->tail->next = link;
      retain(link);
      list->tail = link;
      retain(list->tail);
    }
  else
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      list->head = link;
      retain(link);
      list->tail = link;
      retain(link);
    }
  list_size++;
  list->size += 1;
}

size_t linked_list_size()
{
  return list_size;
}

