#include <stdlib.h>
#include <stdint.h>

typedef struct allocate alloc_t;

struct allocate
{
  uint8_int ref_count;
  obj *object;
  function1_t destructor;
  size_t cascade_limit;
};

  
void set_cascade_limit(alloc_t *alloc, size_t limit)
{
  alloc->cascade_limit = limit;
}


size_t get_cascade_limit(alloc_t *alloc)
{
  return alloc->cascade_limit;
}
