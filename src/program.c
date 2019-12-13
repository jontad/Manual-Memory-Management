#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct person person_t;
typedef struct link link_t;
typedef struct list list_t;

struct person
{
  char *first_name;
  char *last_name;
  int age;
};

struct link
{
  person_t *current;
  link_t *next;
};

struct list
{
  link_t *first;
  link_t *last;
};


person_t *person_create(char *first_name, char *last_name, int age)
{
  person_t *new_person = calloc(1, sizeof(person_t));
  new_person->first_name = first_name;
  new_person->last_name = last_name;
  new_person->age = age;
  return new_person;
}

link_t *link_create(person_t *current, link_t *next)
{
  link_t *new_link = calloc(1, sizeof(link_t));
  new_link->current = current;
  new_link->next = next;
  return new_link;
}

list_t *list_create(link_t *first, link_t *last)
{
  list_t *list = calloc(1, sizeof(list_t));
  list->first = first;
  list->last = last;
  return list;
}

list_t *cray_party()
{
  person_t *olle = person_create("Olle", "Olsson", 99);
  person_t *bosse = person_create("Bosse", "Bossman", 56);
  person_t *kaffe = person_create("Lindwall", "Zoegas", 105);
  link_t *last = link_create(kaffe, NULL);
  link_t *second = link_create(bosse, last);
  link_t *first = link_create(olle, second);
  list_t *list = list_create(first, last);
  return list;
}

int main()
{
  list_t *party = cray_party();
}
