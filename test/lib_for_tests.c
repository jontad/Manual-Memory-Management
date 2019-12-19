#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <stdio.h>

list_t *list_demo = NULL;
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

void link_destructor(obj *c)
{
  release(((new_link_t *) c)->next);
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
  list_demo = allocate(sizeof(list_t), destructor_linked_list);
  list_demo->head = list_demo->tail = NULL;
  list_demo->size = 0;
  return list_demo;
}

//------linked list functions-------//

void linked_list_append()
{
  //list_t *list = object;

  if(list_demo->tail != NULL)
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      release(list_demo->tail);
      list_demo->tail->next = link;
      retain(link);
      list_demo->tail = link;
      retain(list_demo->tail);
    }
  else
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      list_demo->head = link;
      retain(link);
      list_demo->tail = link;
      retain(link);
    }
  list_size++;
  list_demo->size +=1;
}

size_t linked_list_size()
{
  return list_size;
}

