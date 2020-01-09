#include "../src/refmem.h"
#include "../inlupp2/common.h"
#include <stdio.h>
#include <assert.h>

struct cell
{
  struct cell *cell;
  int i;
  char *string;
};

void cell_destructor(obj *c)
{
  release(((struct cell *) c)->cell);
}

int main(void)
{
  struct cell *c = allocate(sizeof(struct cell), cell_destructor);
  assert(rc(c) == 0);
  retain(c);
  assert(rc(c) == 1);

  c->cell = allocate(sizeof(struct cell), cell_destructor);
  assert(rc(c->cell) == 0);
  retain(c->cell);
  assert(rc(c->cell) == 1);
  struct cell *a = c->cell;
  set_cascade_limit(100);
  for(int i = 0; i < 1000; i++)
    {
      a->cell = allocate(sizeof(struct cell), cell_destructor);
      a = a->cell;
      retain(a);      

    }
  a->cell = NULL;
 
  release(c);

  struct cell *tmp = allocate(sizeof(struct cell), cell_destructor);
  tmp->cell=NULL;
  deallocate(tmp);


  printf("YEAH\n");
  shutdown();
  return 0;
}
