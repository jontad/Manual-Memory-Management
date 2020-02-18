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
* hops: maximum amount of pointers in object (1 byte)
* destructor: pointer to function that destroys object (8 bytes)
* pointer-link: link for linked list to store pointer to object (16 bytes)

* what user want to allocate (n bytes)

All allocated objects have a pointer that is placed in a linked list, which keeps track of each object.  
