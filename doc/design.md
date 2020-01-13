# Design

Reference counting in C  
There are several functions to this program, this text will give a high level overview of the implementation of each function.

`allocate`: This function takes a number of bytes and a function pointer to a destructor (Can be NULL) and calls `allocate_array` with array size 1 and the number of bytes.  

`allocate_array`: This function takes array size, number of bytes per array entry and a function pointer for a destructor.(Can be NULL) It allocates that space on the heap, in addition to that some extra space is allocated for the function pointer, a reference counter and a size counter. The extra space is "placed" before the actual space. The reference counter is one byte, meaning that up to 255 references are allowed. The size counter takes the size modulo the size of a pointer, so a allocated object (struct) can have up to 255 pointers. The pointer to the heap is then moved forward past the metadata so that the user may use the space freely. That pointer is also saved in a linked list.

`retain`: This function takes a void pointer. It will then move backwards to the metadata and increase the reference counter with 1. Null pointers are allowed, pointers that are not obtained by `allocate` or `allocate_array` will crash the program. If one tries to retain over 255 nothing will happen.  

`release`: This function takes a void pointer. It will then move backwards to the metadata and decrease the reference counter with 1. Null pointers are allowed, pointers that are not obtained by `allocate` or `allocate_array` will crash the program. If one tries to release something that is not retained, or release so that the reference count is 0 the object will be deallocated.  
  
`rc`: This function takes a void pointer. It will then move backwards to the metadata and return the value of the reference counter. Calling with a null pointer or a pointer not obtained by `allocate` or `allocate_array` will crash the program.  

`deallocate`: This function takes a void pointer to an object and deallocates the memory at that address. Deallocate is only allowed to be called when a objects reference count is 0, if that is false nothing happens. The function will first try to use a destructor from the objects metadata. If no such function exist we will use the default destructor. This means scanning the object for pointers and comparing them to the pointers in the linked list. If a pointer matches `release` will be called on that pointer. A object is scanned by checking for n pointers inside the object, where n is the size count saved in the metadata. When freeing large linked structures like a linked list with hundreds of entries the cascade limit may be hit. This means that the last pointer to be free'd is instead saved so that it can be free'd later to prevent the program from stalling. This pointer is saved on another linked list.  

`set_cascade_limit`: This function sets the limit on how many linked deallocates can happen in a row. A limit of 0 means that there is no limit.  

`get_cascade limit`: This function returns the current limit.

`cleanup`: This function calls deallocate with all objects that have reference count 0 and doesn't care about the cascade limit. It will work through the entire linked list and check all reference counts.  

`shutdown`: This function calls deallocate with all objects in the linked list regardless of the reference count of that object. It will also deallocate the linked list structure with all pointers and the linked list structure with the cascading frees.
