#pragma once
#include <stdlib.h>

typedef void obj;
typedef void(*function1_t)(obj *);

void retain(obj *object);
void release(obj *object);
size_t rc(obj *object);
obj *allocate(size_t bytes, function1_t destructor);
obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);
void deallocate(obj *object);
void set_cascade_limit(size_t size);
size_t get_cascade_limit();
void cleanup();
void shutdown();
