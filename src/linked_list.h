#pragma once
#include <stdbool.h>
//#include "common_for_linked_list.h"
#include "../inlupp2/common.h"


typedef struct list list_t;
typedef struct link link_t;
//typedef bool(*ioopm_char_predicate)(elem_t value, void *extra);
//typedef void(*ioopm_apply_char_function)(elem_t *value, void *extra);
/// Compares two elements and returns true if they are equal
typedef bool(*eq_function)(elem_t a, elem_t b);




/*************** FUNCTIONS **********************/


list_t *get_cascade_list();
void set_cascade_list_to_null();
list_t *linked_list_get_list();

/// @brief Create a new empty list
/// @return an empty linked list
list_t *ioopm_linked_list_create(eq_function func);

/// @brief Tear down the linked list and return all its memory (but not the memory of the elements)
/// @param list the list to be destroyed
void ioopm_linked_list_destroy(list_t *list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_linked_list_append(list_t *list, elem_t value);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended
/// @param value the value to be appended
void ioopm_linked_list_prepend(list_t *list, elem_t value);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @pre value is of same type as the values in list
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be appended
void ioopm_linked_list_insert(list_t *list, int index, elem_t value);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be appended
/// @return the value from index if removed successfully. Else return invalid input.
option_t ioopm_linked_list_remove(obj *object, int index);

link_t *ioopm_linked_list_remove_link(obj *object, int index);

/// @brief finds index of element
int ioopm_linked_list_position(list_t *list, elem_t element);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return the value from index if successfully existing. Else return invalid input.
option_t ioopm_linked_list_get(obj *object, int index);

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param element the element sought
/// @return true if element is in the list, else false
bool ioopm_linked_list_contains(list_t *list, elem_t value);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_linked_list_size(list_t *list);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_linked_list_is_empty(list_t *list);

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ioopm_linked_list_clear(obj *object);


/*
/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ioopm_linked_list_all(list_t *list, predicate prop, void *extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ioopm_linked_list_any(list_t *list, predicate prop, void *extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param fun the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of fun
void ioopm_linked_apply_to_all(list_t *list, apply_function fun, void *extra);
*/
