/**************** LIBRARIES ****************/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"
#include "refmem.h"
#include "common_for_linked_list.h"
//#include "../inlupp2/common.h"

/*************** FUNCTIONS **********************/

list_t *ioopm_linked_list_create(eq_function func)
{
  list_t *list = calloc(1,sizeof(list_t));
  list->first = NULL;
  list->last = NULL;
  list->list_size = 0;
  list->equal = func;
  return list;
}


void ioopm_linked_list_destroy(list_t *list)
{
  ioopm_linked_list_clear(list);
  free(list);
}

static link_t *create_link(list_t *list, elem_t element, link_t *next)
{
  link_t *link = calloc(1,sizeof(link_t));//allocate(sizeof(link_t), NULL);
  link->value = element;
  link->next = next;
  list->list_size = list->list_size + 1;

  return link;
}

void ioopm_linked_list_append(list_t *list, elem_t element)
{
  if(list->last == NULL){
    list->last = create_link(list, element, NULL);
    list->first = list->last;
  } else {
    list->last->next = create_link(list, element, NULL);
    list->last = list->last->next;
  }
}

void ioopm_linked_list_prepend(list_t *list, elem_t element){
  if(list->first == NULL && list->last == NULL){
    list->first = create_link(list, element, NULL);
    list->last = list->first;
  } else {
    list->first = create_link(list, element, list->first);
  }
}


static link_t *find_previous_link(list_t *list, int index){

  link_t *cursor = list->first;
  int counter = 0;

  while(cursor->next != NULL){
    ++counter;

    if(counter == index)
    {
      return cursor;
    } else {
      cursor = cursor->next;
    }
  }
  return cursor;
}

void ioopm_linked_list_insert(list_t *list, int index, elem_t element){
  if (index > ioopm_linked_list_size(list))
  {
    puts("Invalid index input");
  }
  else
  {
    if(index == 0){
      ioopm_linked_list_prepend(list, element);
    } else if(index == ioopm_linked_list_size(list)){
      ioopm_linked_list_append(list, element);
    } else {
        link_t *prev_link = find_previous_link(list, index);
        prev_link->next = create_link(list, element, prev_link->next);
    }
  }
}



void link_destroy(list_t *list, link_t *link)
{
  list->list_size = list->list_size - 1;
  free(link);
  //deallocate(link);
}

static void remove_first(list_t *list)
{
  link_t *link_rm = list->first;
  list->first = list->first->next;
  link_destroy(list, link_rm);
}

static void remove_last(list_t *list, link_t *prev)
{
  link_t *link_rm = list->last;
  prev->next = NULL;
  list->last = prev;
  link_destroy(list, link_rm);
}

static void remove_next(list_t *list, link_t *prev)
{
  link_t *link_rm = prev->next;
  prev->next = prev->next->next;
  link_destroy(list, link_rm);
}

option_t ioopm_linked_list_remove(obj *object, int index)
{
  list_t *list = object;
  bool ok = (!ioopm_linked_list_is_empty(list)) &&
            (index < ioopm_linked_list_size(list)) &&
            (index >= 0);
  if (ok)
  {
    if (index == 0)
    {
      elem_t return_value = list->first->value;

      if (ioopm_linked_list_size(list) == 1)
      {
        link_destroy(list, list->first);
        list->first = NULL;
        list->last = NULL;
      }
      else
      {
        remove_first(list);
      }

      return Success(return_value);
    }
    else
    {
      link_t *prev = find_previous_link(list, index);
      elem_t return_value = prev->next->value;

      if (prev->next == list->last)
      {
        remove_last(list, prev);
      }
      else
      {
        remove_next(list, prev);
      }

      return Success(return_value);
    }
  }
  else
  {
    return Failure();
  }
}

link_t *ioopm_linked_list_remove_link(obj *object, int index)
{
  list_t *list = object;
  bool ok = (!ioopm_linked_list_is_empty(list)) &&
            (index < ioopm_linked_list_size(list)) &&
            (index >= 0);
  if (ok)
  {
    if (index == 0)
    {
      link_t *return_value = list->first;

      if (ioopm_linked_list_size(list) == 1)
      {
        link_destroy(list, list->first);
        list->first = NULL;
        list->last = NULL;
      }
      else
      {
        remove_first(list);
      }

      return return_value;
    }
    else
    {
      link_t *prev = find_previous_link(list, index);
      link_t *return_value = prev->next;

      if (prev->next == list->last)
      {
        remove_last(list, prev);
      }
      else
      {
        remove_next(list, prev);
      }

      return return_value;
    }
  }
  else
  {
    return NULL;
  }
}

option_t ioopm_linked_list_get(obj *object, int index)
{
  list_t *list = object;
  int i = 0;
  link_t *link = list->first;
  while (link != NULL)
  {
    if (i == index)
    {
      return Success(link->value)
    }
    link = link->next;
    i++;
  }

  return Failure();
}

bool ioopm_linked_list_contains(list_t *list, elem_t element)
{
  link_t *link = list->first;

  while (link != NULL)
  {
    if (list->equal(link->value, element))
    {
      return true;
    }
    link = link->next;
  }
  return false;
}

int ioopm_linked_list_position(list_t *list, elem_t element)
{
  link_t *link = list->first;
  int counter = 0;
  while (link != NULL)
  {
    if (list->equal(link->value, element))
    {
      return counter;
    }
    counter++;
    link = link->next;
  }
  return -1;
}



size_t ioopm_linked_list_size(list_t *list)
{
  return list->list_size;
}


bool ioopm_linked_list_is_empty(list_t *list)
{
  return ioopm_linked_list_size(list) == 0;
}


void ioopm_linked_list_clear(obj *object)
{
  list_t *list = object;
  link_t *link = list->first;
  while (link != NULL)
  {
    link_t *temp = link;
    link = link->next;
    link_destroy(list, temp);
  }
  list->first = NULL;
}

/*
bool ioopm_linked_list_all(list_t *list, ioopm_predicate prop, void *extra)
{
  link_t *link = list->first;
  int i = 0;

  while (link != NULL)
  {
    if (prop(int_elem(i), link->value, extra))
    {
      return false;
    }
    link = link->next;
    i++;
  }
  return true;
}

bool ioopm_linked_list_any(list_t *list, ioopm_predicate prop, void *extra)
{
  link_t *link = list->first;
  int i = 0;

  while (link != NULL)
  {
    if (prop(int_elem(i), link->value, extra))
    {
      return true;
    }
    link = link->next;
    i++;
  }
  return false;
}

void ioopm_linked_apply_to_all(list_t *list, ioopm_apply_function apply_fun, void *extra)
{
  link_t *link = list->first;
  int i = 0;

  while (link != NULL)
  {
    apply_fun(int_elem(i), &link->value, extra);
    link = link->next;
    i++;
  }
}
*/
