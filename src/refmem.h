#ifndef __REFMEM_H__
#define __REFMEM_H__

#include <stdlib.h>
#include "../inlupp2/common.h"

#define Free(ptr) {free(ptr); ptr = NULL;}
/**
 * @file refmem.h
 * @author Elias Insulander, Jonathan Tadese, Robert Paananen, Daniel Westberg, Alex Kangas, Georgios Davakos, Joel Waldenbäck
 * @date 11 December 2019
 * @brief Manual memory management using reference counting. 
 * @see http://wrigstad.com/ioopm19/projects/project1.html
 */

/// @brief Renaming void to object
typedef void obj;

/// @brief Destructor function that recieves an object to deallocate
/// @param Object that will be destroyed
typedef void(*function1_t)(obj *);

/// @brief Create an empty linked list that will store pointers to all allocated objects
ioopm_list_t *create_list();

/// @brief Increments reference count of object by 1
/// @param Object where reference count will be increased
/// NOTE: Calling retain() with NULL is supported, however it is ignored
void retain(obj *);

/// @brief Decrements reference count of object by 1. If reference counter is 1 object is free'd
/// @param Object where reference count will be decreased
/// NOTE: Calling release() with NULL is supported, however it is ignored
void release(obj *);

/// @brief Get reference count of object
/// @param Object where the reference count will be retrieved
/// @return Reference count
size_t rc(obj *);

/// @brief Allocate memory on heap 
/// @param bytes Size on heap that will be allocated
/// @param destructor Associated destructor to free what will be allocated 
/// @return Allocated object
obj *allocate(size_t bytes, function1_t destructor);

/// @brief Allocate array of objects in memory on heap 
/// @param bytes Size on heap that will be allocated per element
/// @param destructor Associated destructor to free what will be allocated
/// @param elements How many elements in array
/// @return Allocated array of objects
obj *allocate_array(size_t elements, size_t bytes, function1_t destructor);

/// @brief Deallocate object if reference count is 0
/// @param obj Object that will be deallocated
/// @pre Object has to have reference count 0
void deallocate(obj *);

/// @brief Auxillary function for deallocate. Deallocate object regardless of reference count. 
/// @param obj Object that will be deallocated
void deallocate_aux(obj *);

/// @brief Set upper limit of how many objects are allowed to be free'd at once
/// @param Limit that will be set
void set_cascade_limit(size_t);

/// @brief Get the current cascade limit
/// @return Current cascade limit
size_t get_cascade_limit();

/// @brief Frees all objects with reference count 0, regardless of cascade limit
void cleanup();

/// @brief Completely tears down library and associated data
void shutdown();

/// @brief Gets the list with all object pointers.
ioopm_list_t *linked_list_get();
#endif
