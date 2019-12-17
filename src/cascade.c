#include <stdlib.h>
#include <assert.h>

size_t cascade_limit = 0;
 
void set_cascade_limit(size_t limit)
{
  assert(limit >= 0 && "Can not set cascade limit to a negative value");
  cascade_limit = limit;
}


size_t get_cascade_limit()
{
  return cascade_limit;
}
