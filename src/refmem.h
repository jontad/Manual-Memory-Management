#pragma once
#include <stdlib.h>


#define Free(ptr) {free(ptr); ptr = NULL;}
/**
 * @file refmem.h
 * @author Elias Insulander, Jonathan Tadese, Robert Paananen, Daniel Westberg, Alex Kangas, Georgios Davakos, Joel Waldenbäck
 * @date 11 December 2019
 * @brief Manual memory management using reference counting.
 *
 * More indepth explanation here of what the header defines
 * 
 * @see http://wrigstad.com/ioopm19/projects/project1.html
 */

/// @brief 
typedef void obj;

/// @brief
/// @param
typedef void(*function1_t)(obj *);

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

/// @brief 
/// @param bytes
/// @param destructor
/// @return

obj *allocate(size_t bytes, function1_t destructor);

/// @brief 
/// @param elements
/// @param elem_size
/// @param destructor
/// @return
obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);

/// @brief 
/// @param obj
void deallocate(obj *);

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
