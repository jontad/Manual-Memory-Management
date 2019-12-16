#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <assert.h>
#include <stdio.h>

//Copy
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
  destructor_string(link->elem);
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

void linked_list_append(obj *obj1, obj *obj2)
{
  list_t *list = (list_t *) obj1;

  if(list->tail != NULL)
    {
      link_t *link = allocate(sizeof(link_t), destructor_link);
      link->next = NULL;
      link->elem = obj2;
      list->tail->next = link;
      list->tail = link;
    }
  else
     {
      link_t *link = allocate(sizeof(link_t), destructor_link);
      link->next = NULL;
      link->elem = obj2;
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
