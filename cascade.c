#include <stdlib.h>

size_t cascade_limit = 0;
 
void set_cascade_limit(size_t limit)
{
  cascade_limit = limit;
}


size_t get_cascade_limit()
{
  return cascade_limit;
}
