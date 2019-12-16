/**************** LIBRARIES ****************/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"
#include "refmem.h"

/*************** FUNCTIONS **********************/

void destructor_linked_list_2(obj *object)
{
  ioopm_linked_list_clear(object);
}

ioopm_list_t *ioopm_linked_list_create()
{
  ioopm_list_t *list = allocate(sizeof(ioopm_list_t), destructor_linked_list_2);
  list->first = NULL;
  list->last = NULL;
  list->size = 0;
  list->eq_fun = NULL;
  return list;
}


void ioopm_linked_list_destroy(ioopm_list_t *list)
{
  ioopm_linked_list_clear(list);
  free(list);
}

static link_t *create_link(ioopm_list_t *list, elem_t element, link_t *next)
{
  link_t *link = allocate(sizeof(link_t), NULL);
  link->element = element;
  link->next = next;
  list->size = list->size + 1;

  return link;
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t element)
{
  if(list->last == NULL){
    list->last = create_link(list, element, NULL);
    list->first = list->last;
  } else {
    list->last->next = create_link(list, element, NULL);
    list->last = list->last->next;
  }
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t element){
  if(list->first == NULL && list->last == NULL){
    list->first = create_link(list, element, NULL);
    list->last = list->first;
  } else {
    list->first = create_link(list, element, list->first);
  }
}


static link_t *find_previous_link(ioopm_list_t *list, int index){

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

void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t element){
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



void link_destroy(ioopm_list_t *list, link_t *link)
{
  list->size = list->size - 1;
  free(link);
}

static void remove_first(ioopm_list_t *list)
{
  link_t *link_rm = list->first;
  list->first = list->first->next;
  link_destroy(list, link_rm);
}

static void remove_last(ioopm_list_t *list, link_t *prev)
{
  link_t *link_rm = list->last;
  prev->next = NULL;
  list->last = prev;
  link_destroy(list, link_rm);
}

static void remove_next(ioopm_list_t *list, link_t *prev)
{
  link_t *link_rm = prev->next;
  prev->next = prev->next->next;
  link_destroy(list, link_rm);
}

option_t ioopm_linked_list_remove(obj *object, int index)
{
  ioopm_list_t *list = object;
  bool ok = (!ioopm_linked_list_is_empty(list)) &&
            (index < ioopm_linked_list_size(list)) &&
            (index >= 0);
  if (ok)
  {
    if (index == 0)
    {
      elem_t return_value = list->first->element;

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
      elem_t return_value = prev->next->element;

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


option_t ioopm_linked_list_get(obj *object, int index)
{
  ioopm_list_t *list = object;
  int i = 0;
  link_t *link = list->first;
  while (link != NULL)
  {
    if (i == index)
    {
      return Success(link->element)
    }
    link = link->next;
    i++;
  }

  return Failure();
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
{
  link_t *link = list->first;

  while (link != NULL)
  {
    if (list->eq_fun(link->element, element))
    {
      return true;
    }
    link = link->next;
  }
  return false;
}


size_t ioopm_linked_list_size(ioopm_list_t *list)
{
  return list->size;
}


bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
  return ioopm_linked_list_size(list) == 0;
}


void ioopm_linked_list_clear(obj *object)
{
  ioopm_list_t *list = object;
  link_t *link = list->first;
  while (link != NULL)
  {
    link_t *temp = link;
    link = link->next;
    link_destroy(list, temp);
  }
  list->first = NULL;
}


bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
  link_t *link = list->first;
  int i = 0;

  while (link != NULL)
  {
    if (prop(int_elem(i), link->element, extra))
    {
      return false;
    }
    link = link->next;
    i++;
  }
  return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
  link_t *link = list->first;
  int i = 0;

  while (link != NULL)
  {
    if (prop(int_elem(i), link->element, extra))
    {
      return true;
    }
    link = link->next;
    i++;
  }
  return false;
}

void ioopm_linked_apply_to_all(ioopm_list_t *list, ioopm_apply_function apply_fun, void *extra)
{
  link_t *link = list->first;
  int i = 0;

  while (link != NULL)
  {
    apply_fun(int_elem(i), &link->element, extra);
    link = link->next;
    i++;
  }
}
