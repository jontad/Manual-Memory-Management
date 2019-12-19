#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "linked_list.h"
#include "iterator.h"
#include "common.h"
#include "../src/refmem.h"



///////////////////////////// STRUCTS ///////////////////////////////

struct iter
{
  ioopm_link_t *current;
  ioopm_list_t *list;
};

////////////////////////////////////////////////////////////


static ioopm_link_t *entry_create(ioopm_list_t* list, ioopm_link_t *next, elem_t value) //int
{
  ioopm_link_t *link = allocate(sizeof(ioopm_link_t), NULL);
  retain(link);
  link->value = value;
  link->next = next;
  retain(link->next);
  ++list->list_size;
  return link;
}


static ioopm_link_t *find_previous_entry_for_index(ioopm_list_t *list, int index) //int
{
  ioopm_link_t *entry = list->first;
  retain(entry);
  for(int i = 0; i < index-1; ++i) 
    {
      release(entry);
      entry = entry->next;
      retain(entry);
    }
  return entry;
}


static void list_destroy(ioopm_list_t *list)
{
  release(list);
}


static void link_destroy(ioopm_link_t *link)
{
  release(link);
}


static void insert_aux(ioopm_list_t *list, int index, elem_t value)
{
  ioopm_link_t *previous_entry = find_previous_entry_for_index(list, index);
  ioopm_link_t *entry = entry_create(list, previous_entry->next, value);
  
  previous_entry->next = entry;
  retain(previous_entry->next);

  release(previous_entry);
  release(entry);
}



static elem_t remove_aux(ioopm_list_t *list, int index, ioopm_link_t *prev, ioopm_link_t *elem)
{
  elem_t value = elem->value;

  prev->next = elem->next;
  link_destroy(elem);

  --list->list_size;
  
  return value;
}

static ioopm_link_t *remove_link(ioopm_list_t *list, int index, ioopm_link_t *prev, ioopm_link_t *elem)
{
  ioopm_link_t *link = elem;
  retain(link);

  prev->next = elem->next;
  link_destroy(elem);

  --list->list_size;
  
  return link;
}


ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function equal) 
{
  ioopm_list_t *linked_list = allocate(sizeof(ioopm_list_t), NULL);
  retain(linked_list);
  linked_list->first = NULL;
  linked_list->last = NULL;
  linked_list->equal = equal;
  linked_list->list_size = 0;
  return linked_list;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value)
{
  ioopm_link_t *new_entry = entry_create(list, list->first, value);

  if(list->first == NULL)
    {
      list->last = new_entry;
      retain(new_entry);
    }
  list->first = new_entry;
  retain(new_entry);
}


void ioopm_linked_list_append(ioopm_list_t *list, elem_t value)
{
  if(list->last == NULL)
    {
      ioopm_linked_list_prepend(list, value);
    }
  else
    {
      ioopm_link_t *new_entry = entry_create(list, NULL, value);
      list->last->next = new_entry;
      retain(new_entry);
      release(list->last);
      list->last = new_entry;
      retain(new_entry);
    }
}



void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value)
{
  
  if(list->list_size == 0 || index < 1)
    {
      ioopm_linked_list_prepend(list, value);
    }
  else if(index > 0 && index < list->list_size) 
    {
      insert_aux(list, index, value);
    }
 
  else
    {
      ioopm_linked_list_append(list, value);
    }
}




ioopm_link_t *ioopm_linked_list_remove_link(obj *object, int index)
{
  ioopm_list_t *list = object;
  retain(list);
  ioopm_link_t *prev_element = find_previous_entry_for_index(list, index);
  ioopm_link_t *link = prev_element->next;
  retain(link);
  
  if (index == 0)
    {
      link = prev_element;
      list->first = prev_element->next;
      link_destroy(prev_element);
      --list->list_size;
      if (list->list_size == 0)
	{
	  release(list->last);
	  list->last = NULL;
	}
    }
  else if(link != NULL)
    {
      link = remove_link(list, index, prev_element, link);
    }
  return link;
}


elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index)
{
  ioopm_link_t *prev_element = find_previous_entry_for_index(list, index);
  ioopm_link_t *element = prev_element->next;
  elem_t value;
  
  if (index == 0)
    {
      value = prev_element->value;
      list->first = prev_element->next;
      link_destroy(prev_element);
      --list->list_size;
      if (list->list_size == 0)
	{
	  release(list->last);
	  list->last = NULL;
	}

    }
  else if(element != NULL)
    {
      value = remove_aux(list, index, prev_element, element);
    }
  release(prev_element);
  release(element);
  
  return value;
}

elem_t ioopm_linked_list_get(ioopm_list_t *list, int index)
{
  if(index == 0)
    {
      return list->first->value;
    }
  
  ioopm_link_t *previous_element = find_previous_entry_for_index(list,index);
  elem_t value = previous_element->next->value;
  return value;
}


bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t value)
{
  ioopm_link_t *link = list->first;
  for(int i = 0; i < list->list_size; ++i)
    {
      if(list->equal((link->value), value))
	{
	  return true;
	}
      link = link->next;
      
    }
  return false;
}

int ioopm_linked_list_position(ioopm_list_t *list, elem_t element)
{
  ioopm_link_t *link = list->first;
  retain(link);
  int counter = 0;
  while (link != NULL)
  {
    if (list->equal(link->value, element))
    {
      release(link);
      return counter;
    }
    counter++;
    release(link);
    link = link->next;
    retain(link);
  }
  release(link);
  return -1;
}

size_t ioopm_linked_list_size(ioopm_list_t *list)
{
  return list->list_size;
}



bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
  if(list->list_size == 0)
    {
      return true;
    }
  else
    {
      return false;
    }
}





void ioopm_linked_list_clear(ioopm_list_t *list)
{
  ioopm_link_t *link = list->first;
  retain(link);
  ioopm_link_t *tmp = NULL;
  
  while(link != NULL)
    {
      tmp = link;
      retain(tmp);
      release(link);
      link = link->next;
      if (link != NULL) retain(link);
      link_destroy(tmp);
      
    }
}


void ioopm_linked_list_destroy(ioopm_list_t *list)
{
  ioopm_linked_list_clear(list);
  list_destroy(list);
}



bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_char_predicate prop,  void *extra)
{
  ioopm_link_t *link = list->first;
  for (int i = 0; i < ioopm_linked_list_size(list); ++i)
    {
      if(!(prop(link, extra))) return false;
      link = link->next;
    }
  return true;

}


bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_char_predicate prop, void *extra)
{
  ioopm_link_t *link = list->first;
  for (int i = 0; i < ioopm_linked_list_size(list); ++i)
    {
      if(prop(link, extra)) return true;
      link = link->next;
    }
  return false;
}

  
void ioopm_linked_apply_to_all(ioopm_list_t *list, ioopm_apply_char_function fun, void *extra)
{
  ioopm_link_t *link = list->first;

  while(link != NULL)
    {
      fun(&link, extra);
      link = link->next;
    }
}

////////////////////////////////////////////



ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
  ioopm_list_iterator_t *iter = allocate(sizeof(ioopm_list_iterator_t), NULL);
  iter->current = list->first;
  retain(iter->current);
  iter->list = list;
  retain(iter->list);
  return iter;
}



bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
  if(iter->current->next != NULL)
    {
      return true;
    }
  return false;
}


elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
  release(iter->current);
  iter->current = iter->current->next;
  retain(iter->current);
  return iter->current->value;
}


void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
  release(iter->current);
  iter->current = iter->list->first;
  retain(iter->current);
}



elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
  return iter->current->value;
}


void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
  release(iter);
}



elem_t test_iterator_func(ioopm_list_t *list, int index)
{
  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  for(int i = 0; i<index; ++i)
    {
      if(!ioopm_iterator_has_next(iter))
	{
	  break;
	}
      ioopm_iterator_next(iter);
    }

  ioopm_link_t *elem = iter->current;
  retain(elem);
  
  ioopm_iterator_reset(iter);
  for(int i = 0; i<index-1; ++i)
    {
      if(!ioopm_iterator_has_next(iter))
	{
	  break;
	}
      ioopm_iterator_next(iter);
    }
  
  
  
  ioopm_link_t *prev = iter->current;
  retain(prev);
  elem_t value;
  
  if (index == 0)
    {
      value = prev->value;
      list->first = prev->next;
      link_destroy(prev);
      --list->list_size;
    }
  else if(elem != NULL)
    {
      value = remove_aux(list, index, prev, elem);
    }
  ioopm_iterator_destroy(iter);
  release(elem);
  release(prev);
  return value;
}


