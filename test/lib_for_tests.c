#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"

void destructor_string(obj *object)
{
  char **ptr = object;
  char *str = *ptr;
  free(str);
}
