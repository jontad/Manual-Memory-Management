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
  deallocate(link);
  //new_link_t *tmp;
  /*while (link != NULL)
    {
      tmp = link->next;
      free(link->str);
      deallocate(link);
      link = tmp;
    }
  */
}

list_t *list_cascade = NULL;
size_t list_size = 0;

void list_negate()
{
  list_size--;
}

void link_destructor(obj *c)
{
  new_link_t *link = (new_link_t *) c;
  free(link->str);
  list_size--;
  release(link->next);
}

list_t *list_create()
{
  list_cascade = allocate(sizeof(list_t), destructor_linked_list);
  list_cascade->head = list_cascade->tail = NULL;
  list_cascade->size = 0;
  return list_cascade;
}

//------linked list functions-------//

void linked_list_append(char *str)
{
  //list_t *list = object;

  if(list_cascade->tail != NULL)
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      link->str = str;
      list_cascade->tail->next = link;
      list_cascade->tail = link;
      retain(link);
    }
  else
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      link->str = str;
      list_cascade->head = link;
      list_cascade->tail = link;
      retain(link);
    }
  list_size++;
  list_cascade->size +=1;
}

size_t linked_list_size()
{
  return list_size;
}

