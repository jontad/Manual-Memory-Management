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

void link_destructor(obj *c)
{
  release(((new_link_t *) c)->next); //Det är den här som gör att loopen fortsätter. Vi gör en deallocate på den här länken efter att vi gått in och gjort en deallocate på nästa... Detta betyder att list->head aldrig tas bort?
  //release(c);
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
      list->tail->next = link;
      list->tail = link;
      retain(link);
    }
  else
    {
      new_link_t *link = allocate(sizeof(new_link_t), link_destructor);
      link->next = NULL;
      list->head = link;
      list->tail = link;
    }
  list_size++;
  list->size += 1;
}

size_t linked_list_size()
{
  return list_size;
}

/*
new_link_t *linked_list_get(list_t *list, int index)
{
  if (list == NULL) { return NULL; }

  new_link_t *link = list->
  for (int i = 0; i < index; i++)
    {
      
    }
}
*/
