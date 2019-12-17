#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <assert.h>
#include <stdio.h>

//Copy from allocate.c
struct deallocate_queue
{
  size_t amount;
  list_t *list;
};

extern struct deallocate_queue deallocate_queue;

void destructor_string(obj *object)
{
  if (deallocate_queue.amount == 0)
    {
      return;
    }
  char *str = (char *) object;
  Free(str);
  deallocate_queue.amount--;
}

void destructor_link(obj *object)
{
  if (deallocate_queue.amount == 0)
    {
      return;
    }
  link_t *link = (link_t *) object;
  destructor_string(link->elem); //Egentligen borde det här vara en obj så att man kallar på deallocate(obj)...
  if (link->elem != NULL)
    {
      assert(deallocate_queue.amount != 0);
      return;
    }
  if (deallocate_queue.amount > 0)
    {
      Free(link);
      deallocate_queue.amount--;
    }
}


void destructor_linked_list(obj *object)
{
  list_t *list = object;
  link_t *link = list->head;
  link_t *tmp;
  while (link != NULL && deallocate_queue.amount > 0)
    {
      tmp = link->next;
      deallocate(link);
      if (link != NULL)
	{
	  assert(deallocate_queue.amount != 0); //If false something went wrong, crash the program
	  list->head = link;
	  return;
	}
      link = tmp;
      list->head = tmp;
    }
  if (deallocate_queue.amount > 0)
    {
      Free(list);
      deallocate_queue.amount--;
      //TODO: Här är allt borttaget från listan. Problemet är att vi inte tar bort något från deallocate_queue. Så efter ett tag har vi frigjort allting men länkarna i deallocate_queue ligger kvar....
    }
  if (deallocate_queue.amount == 0)
    {
      puts("amount == 0");
    }
}

//------linked list functions-------//

list_t *linked_list_create()
{
  list_t *list = allocate(sizeof(list_t), destructor_linked_list);
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void linked_list_append(list_t *list, obj *obj)
{
  if(list->tail != NULL)
    {
      link_t *link = allocate(sizeof(link_t), destructor_link);
      link->next = NULL;
      link->elem = obj;
      list->tail->next = link;
      list->tail = link;
    }
  else
    {
      //Create the first link of the list
      link_t *link = allocate(sizeof(link_t), destructor_link);
      link->next = NULL;
      link->elem = obj;
      list->head = link;
      list->tail = link;
    }
  list->size += 1;
}

size_t linked_list_size(obj *object)
{
  list_t *list = object;
  size_t size = list->size;
  return size;
}

//Den här funktionen behöver ses över, fungerar den korrekt?
link_t *linked_list_remove(list_t *list, int index)
{
    if (list && list->head)
    {
      link_t **ptr_current = &(list->head);
      link_t *previous;
      for (int i = 0; i < index; i++)
	{
	  previous = *ptr_current;
	  ptr_current = &((*ptr_current)->next);
	}
      obj *link = (*ptr_current)->elem;
      if ((*ptr_current)->next == NULL)
	{
	  list->tail = previous;
	  //deallocate(*ptr_current); //Vi har redan tagit bort objektet
	  *ptr_current = NULL;
	}
      else
	{
	  link_t *tmp;
	  tmp = (*ptr_current)->next;
	  //deallocate(*ptr_current); //Vi har redan tagit bort objektet
	  *ptr_current = tmp;
	}
      return link;
    }
    else
      {
      return NULL;
      }
}
