#include <stdlib.h>
#include <assert.h>

size_t cascade_limit = 0;
 
void set_cascade_limit(size_t limit)
{
  int limit_int = (int) limit;
  if(limit_int >= 0)
    cascade_limit = limit;
}

size_t get_cascade_limit()
{
  return cascade_limit;
}
