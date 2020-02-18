# Design - Reference counting in C

## Short Overview

What is implemented is manual memory management in C, specifically using reference counting.
This means that each object has an associated counter (reference count) that tracks the number of pointers pointing to that object.
When the reference count (rc) of an allocated object is 0, this object is considered garbage to be free'd. 

Traditionally memory management is handled using malloc()/calloc() and free() for allocating and freeing memory.
Using these are quite unforgiving, which is why Team 13 has developed this program, as a safer tool for memory management.

By using our program you don't need to allocate memory for an already used object, but simply add a reference to that objects reference counter.
When you're done with a perticular reference you subtract from the reference counter.
Our program also recognizes that when an object doesn't have any references anymore, it deallocates the object.

Each object can have a destructor provided by the user, that frees data structures. If a destructor is not provided, there is a default destructor implemented that
frees in its stead.

To avoid freeing too large objects, a freeing limit is implemented, which is the upper limit of how many objects are allowed to be free'd at once
and can be adjusted by the user. If the limit is lower than the amount of objects, the remaining object are stored to be free'd later.
They are free'd during the next allocation.
<img src="https://github.com/IOOPM-UU/Team-13/blob/master/proj/overhead.png" title="allocated"></br>
What is allocated for each object is:

* reference count: Amount of pointers to object (1 byte)
* hops: maximum amount of pointers to object (1 byte)
* destructor: pointer to function that destroys object (8 bytes)
* pointer-link: link for linked list to store pointer to object (16 bytes)

* what user want to allocate (n bytes)

All allocated objects has a pointer that is placed in a linked list, which keeps track of each object.  


## Overview of functions
 
There are several functions to this program, this text will give a high level overview of the implementation of each function.

@brief Allocate memory on heap 
@return pointer to allocated memory
`allocate`: This function takes a number of bytes and a function pointer to a destructor (Can be NULL) and calls `allocate_array` with array size 1 and the number of bytes.  


@brief Allocate array of objects in memory on heap 
@return pointer to allocated memory
`allocate_array`: This function takes array size, number of bytes per array entry and a function pointer for a destructor.(Can be NULL) It allocates that space on the heap, in addition to that some extra space is allocated for the function pointer, a reference counter and a size counter. The extra space is "placed" before the actual space. The reference counter is one byte, meaning that up to 255 references are allowed. The size counter takes the size modulo the size of a pointer, so a allocated object (struct) can have up to 255 pointers. The pointer to the heap is then moved forward past the metadata so that the user may use the space freely. That pointer is also saved in a linked list.


@brief Increments reference count of object by 1
NOTE: Calling retain() with NULL is supported, however it is ignored
\pre Using retain when refrence count is 255 is supported but nothing happens.
`retain`: This function takes a void pointer. It will then move backwards to the metadata and increase the reference counter with 1. Null pointers are allowed, pointers that are not obtained by `allocate` or `allocate_array` will crash the program. If one tries to retain over 255 nothing will happen.  


@brief Decrements reference count of object by 1. If reference counter is 1 object is free'd
NOTE: Calling release() with NULL is supported, however it is ignored
\pre Using release when refrence count is 0 is supported but object is just deallocated
`release`: This function takes a void pointer. It will then move backwards to the metadata and decrease the reference counter with 1. Null pointers are allowed, pointers that are not obtained by `allocate` or `allocate_array` will crash the program. If one tries to release something that is not retained, or release so that the reference count is 0 the object will be deallocated.  


@brief Get reference count of object
@return Reference count
`rc`: This function takes a void pointer. It will then move backwards to the metadata and return the value of the reference counter. Calling with a null pointer or a pointer not obtained by `allocate` or `allocate_array` will crash the program.


@brief Deallocate object if reference count is 0
@pre Object has to have reference count 0
`deallocate`: This function takes a void pointer to an object and deallocates the memory at that address. Deallocate is only allowed to be called when a objects reference count is 0, if that is false nothing happens. The function will first try to use a destructor from the objects metadata. If no such function exist we will use the default destructor. This means scanning the object for pointers and comparing them to the pointers in the linked list. If a pointer matches `release` will be called on that pointer. A object is scanned by checking for n pointers inside the object, where n is the size count saved in the metadata. When freeing large linked structures like a linked list with hundreds of entries the cascade limit may be hit. This means that the last pointer to be free'd is instead saved so that it can be free'd later to prevent the program from stalling. This pointer is saved on another linked list.  

@brief Set upper limit of how many objects are allowed to be free'd at once
`set_cascade_limit`: This function sets the limit on how many linked deallocates can happen in a row. A limit of 0 means that there is no limit.  

@brief Get the current cascade limit
`get_cascade limit`: This function returns the current limit.

@brief Frees all objects with reference count 0, regardless of cascade limit
`cleanup`: This function calls deallocate with all objects that have reference count 0 and doesn't care about the cascade limit. It will work through the entire linked list and check all reference counts.  

@brief Completely tears down library and associated data
`shutdown`: This function calls deallocate with all objects in the linked list regardless of the reference count of that object. It will also deallocate the linked list structure with all pointers and the linked list structure with the cascading frees.

'default_destruct' This function is called if an object doesn't have its own destructor. In short it works like this. When an object is removed the default destructor will follow all pointers in the object and check whether the memory should be freed. If rc(object) == 0 the object will be freed. When the cascade_limit has been reached the destructor will stop and the current object will be saved in a linked list. This freeing of memory will resume at a later time. One problem with the current implementation isthat if an objects subobjects aren't "chained", i.e. the subobject which is freed doesn't point to the next subobject which should be freed, no cascade_limit can be set. This is because of the fact that The default destructor always remove the object (even if all subobjects haven't been removed). However, if the objects are "chained" (for example in a linked list) a cascade_limit can be set because the default destructor can pause when cascade_limit has been reached and at a later time continue removing all links in the list even though the list struct has been freed since the link removed points to the next link which will be removed.
