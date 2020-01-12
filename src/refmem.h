#ifndef __REFMEM_H__
#define __REFMEM_H__

#include <stdlib.h>
//#include "common_for_linked_list.h"
//#include "linked_list.h"
#define Free(ptr) {free(ptr); ptr = NULL;}

//global functions to set, clear and test a bit in a bit array
//the current "hash function" where we do k%bit_array_size to find the row in A and k%row_size to find the bit in the row has its disadvantages. Mainly that several different pointers can hash to the same bit in the bit array. We used this hash function because we couldn't determine the size needed for bit_array to be able to do k/bit_array_size in order to find the row in A.
#define bit_array_size 50000
#define row_size 64
#define set_bit(A,k)     ( A[((long)(char *)k%bit_array_size)] |= ((long)1 << ((long)(char *)k%row_size)) )
#define clear_bit(A,k)   ( A[((long)(char *)k%bit_array_size)] &= ~((long)1 << ((long)(char *)k%row_size)) )
#define test_bit(A,k)    ( A[((long)(char *)k%bit_array_size)] & ((long)1 << ((long)(char *)k%row_size)) )
#define le_bit(num,k) ( num & ((long)1 << (long)k) )
/*
#define set_bit(A,k)     ( A[((long)k/32)] |= (1 << ((long)k%32)) )
#define clear_bit(A,k)   ( A[((long)k/32)] &= ~(1 << ((long)k%32)) )
#define test_bit(A,k)    ( A[((long)k/32)] & (1 << ((long)k%32)) )
*/

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

obj **get_pointer_array();

long *get_bit_array();

int allocs_in_bit_array();

#endif
