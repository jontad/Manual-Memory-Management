#ifndef _ITERATORH_
#define _ITERATORH_

#include <stdbool.h>
#include "common.h"


/// @brief Create an iterator for a given list
/// @param list: the list to be iterated over
/// @return an iterator positioned at the start of list
list_iterator_t *list_iterator(list_t *list);

/// @brief Checks if there are more elements to iterate over
/// @param iter: the iterator
/// @return true if the iterator have more elements to iterate over, else false
bool iterator_has_next(list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter: the iterator
/// @return the value of the next element
elem_t iterator_next(list_iterator_t *iter);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter: the iterator
void iterator_reset(list_iterator_t *iter);

/// @brief Retrives the current element from the underlying list
/// @param iter: the iterator
/// @return the value of the current element
elem_t iterator_current(list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter: the iterator
void iterator_destroy(list_iterator_t *iter);

#endif
