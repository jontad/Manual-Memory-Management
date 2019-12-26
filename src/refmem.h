#ifndef __REFMEM_H__
#define __REFMEM_H__

#include <stdlib.h>
//#include "common_for_linked_list.h"
//#include "linked_list.h"
#define Free(ptr) {free(ptr); ptr = NULL;}

//global functions to set, clear and test a bit in a bit array
#define bit_array_size 32
#define set_bit(A,k)     ( A[(k/bit_array_size)] |= (1 << (k%32)) )
#define clear_bit(A,k)   ( A[(k/bit_array_size)] &= ~(1 << (k%32)) )
#define test_bit(A,k)    ( A[(k/bit_array_size)] & (1 << (k%32)) )

/**
 * @file refmem.h
 * @author Elias Insulander, Jonathan Tadese, Robert Paananen, Daniel Westberg, Alex Kangas, Georgios Davakos, Joel Waldenbäck
 * @date 11 December 2019
 * @brief Manual memory management using reference counting. 
 * @see http://wrigstad.com/ioopm19/projects/project1.html
 */

/// @brief Renaming void to obj
typedef void obj;

/// @brief Destructor function that recieves an object to deallocate
/// @param object Object that will be destroyed
typedef void(*function1_t)(obj *object);

/// @brief Increments reference count of object by 1
/// @param object Object where reference count will be increased
/// NOTE: Calling retain() with NULL is supported, however it is ignored
/// \pre Using retain when refrence count is 255 is supported but nothing happens.
void retain(obj *object);

/// @brief Decrements reference count of object by 1. If reference counter is 1 object is free'd
/// @param object Object where reference count will be decreased
/// NOTE: Calling release() with NULL is supported, however it is ignored
/// \pre Using release when refrence count is 0 is supported but object is just deallocated
void release(obj *object);

/// @brief Get reference count of object
/// @param Object where the reference count will be retrieved
/// @return Reference count
size_t rc(obj *object);

/// @brief Allocate memory on heap 
/// @param bytes Size on heap that will be allocated
/// @param destructor Associated destructor to free what will be allocated 
/// @return pointer to allocated memory
obj *allocate(size_t bytes, function1_t destructor);

/// @brief Allocate array of objects in memory on heap 
/// @param bytes Size on heap that will be allocated per element
/// @param destructor Associated destructor to free what will be allocated
/// @param elements How many elements in array
/// @return pointer to allocated memory
obj *allocate_array(size_t elements, size_t bytes, function1_t destructor);

/// @brief Deallocate object if reference count is 0
/// @param object Object that will be deallocated
/// @pre Object has to have reference count 0
void deallocate(obj *object);

/// @brief Auxillary function for deallocate. Deallocate object regardless of reference count. 
/// @param object Object that will be deallocated
void deallocate_aux(obj *object);

/// @brief Set upper limit of how many objects are allowed to be free'd at once
/// @param size Limit that will be set
void set_cascade_limit(size_t size);

/// @brief Get the current cascade limit
/// @return Current cascade limit
size_t get_cascade_limit();

/// @brief Frees all objects with reference count 0, regardless of cascade limit
void cleanup();

/// @brief Completely tears down library and associated data
void shutdown();




int* get_bit_array();

obj *allocate_with_bitarray(size_t bytes, function1_t destructor);

obj *allocate_array_with_bitarray(size_t elements, size_t bytes, function1_t destructor);

void default_destruct_with_bitarray(obj *object);

void deallocate_aux_with_bitarray(obj *object);

void deallocate_with_bitarray(obj *object);

void release_with_bitarray(obj *object);

void cleanup_with_bitarray();

void shutdown_with_bitarray();

size_t rc_with_bitarray(obj *object);


#endif
