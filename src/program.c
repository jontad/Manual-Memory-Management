#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "refmem.h"

typedef struct person person_t;

struct person
{
  char *first_name;
  char *last_name;
  int age;
};

void person_kill(obj *person)
{
  free(person);
}

person_t *person_create(char *first_name, char *last_name, int age)
{
  person_t *new_person = allocate(sizeof(person_t), person_kill);
  new_person->first_name = first_name;
  new_person->last_name = last_name;
  new_person->age = age;
  return new_person;
}

void cray_party()
{
  person_t *olle = person_create("Olle", "Olsson", 99);
  person_t *bosse = person_create("Bosse", "Bossman", 56);
  person_t *kalle = person_create("Kalle", "Anka", 123);
  person_t *kaffe = person_create("Zoegas", "Lindwall", 105);
}

int main()
{
  cray_party();
}
