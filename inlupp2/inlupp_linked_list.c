#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "inlupp_linked_list.h"
#include "iterator.h"

#include "common.h"

#include "../src/refmem.h"



///////////////////////////// STRUCTS ///////////////////////////////

struct iter
{
  link_t *current;
  list_t *list;
};

////////////////////////////////////////////////////////////


static link_t *entry_create(list_t* list, link_t *next, elem_t value) //int
{
  link_t *link = allocate(sizeof(link_t), NULL);
  retain(link);
  link->value = value;
  link->next = next;
  //retain(link->next);
  ++list->list_size;
  return link;
}


static link_t *find_previous_entry_for_index(list_t *list, int index) //int
{
  link_t *entry = list->first;
  retain(entry);
  for(int i = 0; i < index-1; ++i) 
    {
      release(entry);
      entry = entry->next;
      retain(entry);
    }
  release(entry);
  return entry;
}


static void list_destroy(list_t *list)
{
  release(list);
}


static void link_destroy(link_t *link)
{
  release(link);
}


static void insert_aux(list_t *list, int index, elem_t value)
{
  link_t *previous_entry = find_previous_entry_for_index(list, index);
  link_t *entry = entry_create(list, previous_entry->next, value);
  
  previous_entry->next = entry;
  //retain(previous_entry->next);

  //release(previous_entry);
  //release(entry);
}

list_t *inlupp_linked_list_create(eq_function equal) 
{
  list_t *linked_list = allocate(sizeof(list_t), NULL);
  retain(linked_list);
  linked_list->first = NULL;
  linked_list->last = NULL;
  linked_list->equal = equal;
  linked_list->list_size = 0;
  return linked_list;
}

void inlupp_linked_list_prepend(list_t *list, elem_t value)
{
  link_t *new_entry = entry_create(list, list->first, value);
  //new_entry has been retained by entry_create()
  //release(list->first);
  
  if(list->last == NULL)
    {
      list->last = new_entry;
      retain(new_entry);
   }
  list->first = new_entry;
  //retain(new_entry);
  //release(new_entry->next);
}


void inlupp_linked_list_append(list_t *list, elem_t value)
{
  if(list->last == NULL)
    {
      inlupp_linked_list_prepend(list, value);
    }
  else
    {
      link_t *new_entry = entry_create(list, NULL, value);
      //new_entry has been retained by entry_create()
      list->last->next = new_entry;
      release(list->last);
      list->last = new_entry;
      retain(list->last);
      //printf("RC: %ld\n", rc(new_entry));
    }
}



void inlupp_linked_list_insert(list_t *list, int index, elem_t value)
{
  
  if(list->list_size == 0 || index < 1)
    {
      inlupp_linked_list_prepend(list, value);
    }
  else if(index > 0 && index < list->list_size) 
    {
      insert_aux(list, index, value);
    }
  else
    {
      inlupp_linked_list_append(list, value);
    }
}

static link_t *remove_link(list_t *list, int index, link_t *prev, link_t *elem)
{
  link_t *link = elem;
  retain(link);

  prev->next = elem->next;
  link_destroy(elem);

  --list->list_size;
  
  return link;
}

link_t *inlupp_linked_list_remove_link(obj *object, int index)
{
  list_t *list = object;
  retain(list);
  link_t *prev_element = find_previous_entry_for_index(list, index);
  link_t *link = prev_element->next;
  retain(link);
  
  if (index == 0)
    {
      link = prev_element;
      list->first = prev_element->next;
      prev_element->next = NULL;
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


static elem_t remove_aux(list_t *list, int index, link_t *prev, link_t *elem)
{
  elem_t value = elem->value;

  prev->next = elem->next;
  link_destroy(elem);

  --list->list_size;
  
  return value;
}

elem_t inlupp_linked_list_remove(list_t *list, int index)
{
  link_t *prev_element = find_previous_entry_for_index(list, index);
  link_t *element = prev_element->next;
  retain(element);
  elem_t value;
  
  if (index == 0)
    {
      value = prev_element->value;
      list->first = prev_element->next;
      prev_element->next = NULL;
      link_destroy(prev_element);
      --list->list_size;
      if (list->list_size == 0)
	{
	  //release(list->last);
	  list->last = NULL;
	}

    }
  else if(element != NULL)
    {
      value = remove_aux(list, index, prev_element, element);
    }
  //release(prev_element); //Inte helt säker på om vi behöver frigöra prev
  release(element);
  
  return value;
}

elem_t inlupp_linked_list_get(list_t *list, int index)
{
  if(index == 0)
    {
      return list->first->value;
    }
  
  link_t *previous_element = find_previous_entry_for_index(list,index);
  elem_t value = previous_element->next->value;
  return value;
}


bool inlupp_linked_list_contains(list_t *list, elem_t value)
{
  link_t *link = list->first;
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

int inlupp_linked_list_position(list_t *list, elem_t element)
{
  link_t *link = list->first;
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

size_t inlupp_linked_list_size(list_t *list)
{
  return list->list_size;
}



bool inlupp_linked_list_is_empty(list_t *list)
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





void inlupp_linked_list_clear(list_t *list)
{
  //  link_t *link = list->first;
  
  release(list->first);  
  release(list->last);

  /*  link_t *tmp = NULL;
  while(link != NULL)
    {
      tmp = link;
      link = link->next;
      //retain(link);
      link_destroy(tmp);  
      }*/

}


void inlupp_linked_list_destroy(list_t *list)
{
  inlupp_linked_list_clear(list);
  list_destroy(list);
}



bool inlupp_linked_list_all(list_t *list, char_predicate prop,  void *extra)
{
  link_t *link = list->first;
  for (int i = 0; i < inlupp_linked_list_size(list); ++i)
    {
      if(!(prop(link, extra))) return false;
      link = link->next;
    }
  return true;

}


bool inlupp_linked_list_any(list_t *list, char_predicate prop, void *extra)
{
  link_t *link = list->first;
  for (int i = 0; i < inlupp_linked_list_size(list); ++i)
    {
      if(prop(link, extra)) return true;
      link = link->next;
    }
  return false;
}

  
void inlupp_linked_apply_to_all(list_t *list, apply_char_function fun, void *extra)
{
  link_t *link = list->first;

  while(link != NULL)
    {
      fun(&link, extra);
      link = link->next;
    }
}

////////////////////////////////////////////



list_iterator_t *list_iterator(list_t *list)
{
  list_iterator_t *iter = allocate(sizeof(list_iterator_t), NULL);
  retain(iter);
  iter->current = list->first;
  retain(iter->current);
  iter->list = list;
  retain(iter->list);
  return iter;
}



bool iterator_has_next(list_iterator_t *iter)
{
  if(iter->current->next != NULL)
    {
      return true;
    }
  return false;
}


elem_t iterator_next(list_iterator_t *iter)
{
  release(iter->current);
  iter->current = iter->current->next;
  retain(iter->current);
  return iter->current->value;
}


void iterator_reset(list_iterator_t *iter)
{
  release(iter->current);
  iter->current = iter->list->first;
  retain(iter->current);
}



elem_t iterator_current(list_iterator_t *iter)
{
  return iter->current->value;
}


void iterator_destroy(list_iterator_t *iter)
{
  release(iter);
}
