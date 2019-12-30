#ifndef _LISTH_
#define _LISTH_

#include <stdbool.h>
#include "common.h"


/**
 * @file linked_list.h
 * @author Fredrik Yngve & Jonathan Tadese
 * @brief Linked list that stores values 
 */


/// @brief Creates a new empty linked list
/// @return an empty linked list
list_t *inlupp_linked_list_create(eq_function equal);

/// @brief Tear down the linked list and return all its memory (but not the memory of the elements)
/// @param list the list to be destroyed
void inlupp_linked_list_destroy(list_t *list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list: the linked list that will be appended
/// @param value: the value to be appended
void inlupp_linked_list_append(list_t *list, elem_t value);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list: the linked list that will be prepended
/// @param value: the value to be prepended
void inlupp_linked_list_prepend(list_t *list, elem_t value);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @param list: the linked list that will be extended
/// @param index: the position in the list
/// @param value: the value to be inserted
void inlupp_linked_list_insert(list_t *list, int index, elem_t value);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list: the linked list that will be shortened
/// @param index: the position in the list
/// @return the removed value
elem_t inlupp_linked_list_remove(list_t *list, int index);

/// @brief Remove a link from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list: the linked list that will be shortened
/// @param index: the position in the list
/// @return the removed link
link_t *inlupp_linked_list_remove_link(obj *object, int index);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list: the linked list that will be used to retrive the value
/// @param index: the position in the list
/// @return the value at the given position
elem_t inlupp_linked_list_get(list_t *list, int index);

/// @brief Check if an element is in the list
/// @param list: the linked list that will be checked
/// @param value: the element sought
/// @return true if element is in the list, else false
bool inlupp_linked_list_contains(list_t *list, elem_t value);

/// @brief finds index of element
/// @param list: the linked list searched
/// @param element: element searched for 
/// @return Found index
int inlupp_linked_list_position(list_t *list, elem_t element);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list: the linked list that will be counted
/// @return the number of elements in the list
size_t inlupp_linked_list_size(list_t *list);

/// @brief Check whether a list is empty or not
/// @param list: the linked list that will be checked
/// @return true if the number of elements int the list is 0, else false
bool inlupp_linked_list_is_empty(list_t *list);

/// @brief Remove all elements from a linked list
/// @param list: the linked list that will be cleared
void inlupp_linked_list_clear(list_t *list);

/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list: the linked list
/// @param prop: the property to be tested (function pointer)
/// @param extra: an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool inlupp_linked_list_all(list_t *list, char_predicate prop, void *extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list: the linked list
/// @param prop: the property to be tested
/// @param extra: an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool inlupp_linked_list_any(list_t *list, char_predicate prop, void *extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list: the linked list
/// @param fun: the function to be applied
/// @param extra: an additional argument (may be NULL) that will be passed to all internal calls of fun
void inlupp_linked_apply_to_all(list_t *list, apply_char_function fun, void *extra);

#endif
