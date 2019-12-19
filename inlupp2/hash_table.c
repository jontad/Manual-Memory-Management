#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "common.h"
#include "../src/refmem.h"


///////////////////////////// STRUCTS ///////////////////////////////

struct entry
{
  elem_t key;       // holds the key
  elem_t value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

////////////////////////////////////////////////////////////




static entry_t *entry_create(elem_t key, elem_t value, entry_t *next)
{
  entry_t *new_entry = allocate(sizeof(entry_t), NULL);
  new_entry->key = key;
  new_entry->value = value;
  new_entry->next = next;
  retain(next);
  
  return new_entry;
}

static void create_dummys(ioopm_hash_table_t *ht)
{
  for (int i = 0 ; i < ht->capacity ; i++)
    {
      elem_t empty = { .ioopm_void_ptr = NULL };
      ht->buckets[i] = entry_create(empty, empty, NULL); // Dummy
      retain(ht->buckets[i]);
    }
}

ioopm_hash_table_t *ioopm_hash_table_create(hash_function hash_func, ioopm_eq_function key_eq, ioopm_eq_function value_eq, float load_factor, size_t capacity)
{

  /// Allocate space for a ioopm_hash_table_t = ht->capacity pointers to
  /// entry_t's, which will be set to NULL
  ioopm_hash_table_t *result = allocate(sizeof(ioopm_hash_table_t), NULL);
  entry_t **buckets = allocate_array(capacity, sizeof(entry_t *), NULL);
  result->buckets = buckets;
  retain(buckets);
  
  result->capacity = capacity;
  create_dummys(result);
  result->hash_func = hash_func;
  result->key_eq_func = key_eq;
  result->value_eq_func = value_eq;
  result->load_factor = load_factor;
  result->size = 0;
  return result;  /// Allocate space for a ioopm_hash_table_t = ht->capacity pointers to entry_t's
}

static void entry_destroy(entry_t *entry)
{
  release(entry);
}


static void buckets_destroy(ioopm_hash_table_t *ht)
{
  for (int i = 0 ; i < ht->capacity ; i++)
    {
      entry_t *current_entry = ht->buckets[i];
      //retain(current_entry);
      entry_t *previous_entry;
      while (current_entry->next != NULL)
	{
	  previous_entry = current_entry;
	  
	  current_entry = current_entry->next;
	  //retain(current_entry);
	  entry_destroy(previous_entry);
	}
      entry_destroy(current_entry);
    }
  release(ht->buckets);
}


static void resize_hash_table(ioopm_hash_table_t *ht)
{
  size_t new_capacity = 0;
  size_t primes[] = {17, 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381};

  for (int i = 1 ; new_capacity <= ht->capacity; ++i)
    {
      new_capacity = primes[i];
    }
  
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  retain(keys);
  ioopm_list_t *values = ioopm_hash_table_values(ht);
  retain(values);
  
  buckets_destroy(ht);
  
  ht->capacity = new_capacity;
  entry_t **new_buckets = allocate_array(ht->capacity, sizeof(entry_t *), NULL);
  ht->buckets = new_buckets;
  retain(new_buckets);

  create_dummys(ht);
  ht->size = 0;
  
  ioopm_link_t *current_key_link = keys->first;
  retain(current_key_link);
  ioopm_link_t *current_value_link = values->first;
  retain(current_value_link);
  
  while (current_key_link != NULL)
    {
      ioopm_hash_table_insert(ht, current_key_link->value, current_value_link->value);
      
      release(current_key_link);
      current_key_link = current_key_link->next;
      retain(current_key_link);
      
      release(current_value_link);
      current_value_link = current_value_link->next;
      retain(current_value_link);
    }
  
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
}

//Took find_previous_entry_for_key from the other hash_table.c, as our segfaulted for the last entry
static entry_t *find_previous_entry_for_key(entry_t *first_entry, elem_t key, hash_function hash_func)
{
  if (!first_entry) return NULL;
  
  while (first_entry->next != NULL && hash_func(first_entry->next->key) < hash_func(key))
    {
      release(first_entry);
      first_entry = first_entry->next;
      retain(first_entry);
    }
  return first_entry;
} 
/*{

  entry_t *current_entry = first_entry;
  entry_t *temp_entry = first_entry;
  while(hash_func(current_entry->key) < hash_func(key) && current_entry->next != NULL)
    {
      temp_entry = current_entry;
      current_entry = current_entry->next;
    }
  if(hash_func(current_entry->key) < hash_func(key) && current_entry->next == NULL)
    {
      return current_entry;
    }
  else
    {
      return temp_entry;
    }

}
*/





void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  /// Calculate the bucket for this entry
  int bucket = ht->hash_func(key) % ht->capacity;
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->buckets[bucket], key, ht->hash_func);
  retain(entry);
  entry_t *next = entry->next;
  retain(next);
  
  /// Check if the next entry should be updated or not
  if (next != NULL && ht->key_eq_func(next->key, key))
    {
      next->value = value;
    }
  else
    {
      entry->next = entry_create(key, value, next);
      retain(entry->next);
      ++ht->size;
      if (ht->size / (float)ht->capacity >= ht->load_factor)
	{
	  resize_hash_table(ht);
	}
    }
  // release(entry/next)????
}


//Took lookup from the other hash_table.c as our had issues (lookup on "namn9" somehow found "namn19")
option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
  entry_t *tmp = find_previous_entry_for_key(ht->buckets[ht->hash_func(key) % ht->capacity], key, ht->hash_func);
  retain(tmp);
  entry_t *next = tmp->next;
  retain(next);
  
  if (next && ht->hash_func(next->key) == ht->hash_func(key))
    {
      return Success(next->value);
    }
  else
    {
      return Failure();
    }
}


/*
option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
  /// Find the previous entry for key
  entry_t *previous_entry = find_previous_entry_for_key(ht->buckets[ht->hash_func(key) % ht->capacity], key, ht->hash_func);
  entry_t *next = previous_entry->next;

  if (next && (next->value.ioopm_int || next->value.ioopm_u_int || next->value.ioopm_bool || next->value.ioopm_float || next->value.ioopm_void_ptr)) // will not work if value is false (boolean)
    {
      return Success(next->value);
    }
  else
    {
      return Failure();
    }
}*/



elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
  option_t exist = ioopm_hash_table_lookup(ht, key);
  elem_t removed_value = str_elem("Non existent key");
  if (exist.success)
    {
      entry_t *previous_entry = find_previous_entry_for_key(ht->buckets[ht->hash_func(key) % ht->capacity], key, ht->hash_func);
      retain(previous_entry)
	
      entry_t *temp_entry = previous_entry->next;
      retain(temp_entry);
      
      removed_value = temp_entry->value;
      
      previous_entry->next = temp_entry->next;
      retain(temp_entry->next);
      
      entry_destroy(temp_entry);
      --ht->size;
    }
  return removed_value;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
  for (int i = 0 ; i < ht->capacity ; i++)
    {
      entry_t *current_entry = ht->buckets[i];
      retain(current_entry);
      entry_t *previous_entry;
      while (current_entry->next != NULL)
	{
	  previous_entry = current_entry;
	  current_entry = current_entry->next;  
	  retain(current_entry);
	  
	  entry_destroy(previous_entry);
	}
      entry_destroy(current_entry);
    }
  release(ht->buckets);
  // free(ht->buckets);
  release(ht);
  // free(ht);
}


size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  return ht->size;
}


bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  for (int i = 0 ; i < ht->capacity ; i++)
    {
      if (ht->buckets[i]->next != NULL)
	{
	  return false;
	}
    }
  return true;
}


void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
  for (int i = 0 ; i < ht->capacity ; i++)
    {
      if (ht->buckets[i]->next != NULL)
	{
	  entry_t *current_entry = ht->buckets[i]->next;
	  retain(current_entry);
	  ht->buckets[i]->next = NULL;
	  entry_t *previous_entry;
	  while (current_entry->next != NULL)
	    {
	      previous_entry = current_entry;
	      
	      current_entry = current_entry->next;
	      retain(current_entry);
	      
	      entry_destroy(previous_entry);
	    }
	  entry_destroy(current_entry);
	}
    }
  ht->size = 0;
}



//REMEMBER:   free(keys) after function.
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
  ioopm_list_t *keys = ioopm_linked_list_create(ht->key_eq_func);
  retain(keys);
  for (int i = 0; i < ht->capacity; ++i)
    {
      if (ht->buckets[i]->next != NULL)
	{
	  entry_t *current_entry = ht->buckets[i]->next;
	  retain(current_entry);
	  while(current_entry != NULL)
	    {
	      ioopm_linked_list_append(keys, current_entry->key);

	      release(current_entry);
	      current_entry = current_entry->next;
	      retain(current_entry);
	    }
	}
    }
  return keys;
}



//REMEMBER:   free(values) after function.
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
  ioopm_list_t *values = ioopm_linked_list_create(ht->value_eq_func);
  retain(values);
  for (int i = 0; i < ht->capacity; ++i)
    {
      if (ht->buckets[i]->next != NULL)
	{
	  entry_t *current_entry = ht->buckets[i]->next;
	  retain(current_entry);
	  while(current_entry != NULL)
	    {
	      ioopm_linked_list_append(values, current_entry->value);

	      release(current_entry);
	      current_entry = current_entry->next;
	      retain(current_entry);
	    }
	}
    }
  return values;
}



bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  bool result = false;
  for (int i = 0; i < ht->capacity && !result; ++i)
    {
      if (ht->buckets[i]->next != NULL)
	{
	  entry_t *current_entry = ht->buckets[i]->next;
	  retain(current_entry);
	  while(current_entry != NULL && !result)
	    {
	      result = ht->key_eq_func(current_entry->key, key);

	      release(current_entry);
	      current_entry = current_entry->next;
	      retain(current_entry);
	    }
	}
    }
  return result;
}



bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
  bool result = false;
  for (int i = 0; i < ht->capacity && !result; ++i)
    {
      if (ht->buckets[i]->next != NULL)
	{
	  entry_t *current_entry = ht->buckets[i]->next;
	  retain(current_entry);
	  while(current_entry != NULL && !result)
	    {
	      result = ht->value_eq_func(current_entry->value, value);

	      release(current_entry);
	      current_entry = current_entry->next;
	      retain(current_entry);
	    }
	}
    }
  return result;
}


bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
  bool result = true;
  size_t size = ioopm_hash_table_size(ht);

  for(int i = 0; i < size; ++i)
    {
      entry_t *entry = ht->buckets[i];
      retain(entry);
      while(entry->next != NULL)
	{
	  if(!pred(entry->next->key, entry->next->value, arg))
	    {
	      return false;
	    }
	  release(entry);
	  entry = entry->next;
	  retain(entry);
	} 
    }
  return result;  
}


bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
  bool result = false;
  size_t size = ioopm_hash_table_size(ht);

  for(int i = 0; i < size; ++i)
    {
      entry_t *entry = ht->buckets[i];
      retain(entry);
      while(entry->next != NULL)
	{
	  if(pred(entry->next->key, entry->next->value, arg))
	    {
	      return true;
	    }
	  release(entry);
	  entry = entry->next;
	  retain(entry);
	} 
    }
  return result;
}


void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
  for (int i = 0 ; i < ht->capacity ; i++)
    {
      entry_t *current_entry = ht->buckets[i];
      retain(current_entry);
      while(current_entry->next != NULL)
	{
	  release(current_entry);
	  current_entry = current_entry->next;
	  retain(current_entrya);
	  
	  apply_fun(current_entry->key, &current_entry->value, arg);
	}
    }
}




unsigned long int_hash_func(elem_t key)
{
  return key.ioopm_int;
}
  
unsigned long uns_int_hash_func(elem_t uns_int)
{
  int result = uns_int.ioopm_u_int;
  return result;
}

  
unsigned long bool_hash_func(elem_t boolean)
{
  if(boolean.ioopm_bool)
    {
      return 1;
    }
  return 0;
}

unsigned long float_hash_func(elem_t float_num)
{
  unsigned long result = float_num.ioopm_float / 2;
  return result;
}

unsigned long str_hash_func(elem_t string)
{
  unsigned long result = 0;
  char *str = string.ioopm_void_ptr;
  int i = 0;
  if (str == NULL)
    {
      return result;
    }
  while (str[i] != '\0')
    {
      result = result * 31 + str[i];
      ++i;
    }
  return result;
}

unsigned long pointer_hash_func(elem_t pointer)
{
  return 1; //worthless hash function test
}

unsigned long merch_hash_func(elem_t merchandise)
{
  return str_hash_func(str_elem(merchandise.ioopm_merch->name));
}
 

bool equality_function_int(elem_t a, elem_t b)
{
  return a.ioopm_int == b.ioopm_int;
}
  
bool equality_function_uns_int(elem_t a, elem_t b)
{
  return a.ioopm_u_int == b.ioopm_u_int;
}
  
bool equality_function_bool(elem_t a, elem_t b)
{
  return (a.ioopm_bool && b.ioopm_bool) || (!a.ioopm_bool && !b.ioopm_bool);
}

bool equality_function_float(elem_t a, elem_t b)
{
  return a.ioopm_float == b.ioopm_float;
}
  
bool equality_function_str(elem_t a, elem_t b)
{
  if (strcmp(a.ioopm_void_ptr, b.ioopm_void_ptr) == 0)
    {
      return true;
    }
  return false;
}
  
bool equality_function_pointer(elem_t a, elem_t b)
{
  if (a.ioopm_void_ptr == b.ioopm_void_ptr)
    {
      return true;
    }
  return false;
}

bool equality_function_merch(elem_t a, elem_t b)
{
  elem_t merch1 = str_elem(a.ioopm_merch->name);
  elem_t merch2 = str_elem(b.ioopm_merch->name);    
  return equality_function_str(merch1, merch2);
}

