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
