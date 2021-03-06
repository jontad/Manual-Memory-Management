#include <stdbool.h>
#include "../src/refmem.h"
#include "lib_for_tests.h"
#include <stdio.h>

new_list_t *list = NULL;

size_t list_size = 0;

void lib_for_tests_destructor_string(obj *object)
{
  char **ptr = object;
  char *str = *ptr;
  free(str);
}

void lib_for_tests_destructor_string_array(obj *object)
{
  char **array_ptr = object;
  for(int i = 0; i < 10; i++)
    {
      free(array_ptr[i]);
    }
}

void lib_for_tests_destructor_linked_list(obj *object)
{
  new_list_t *list = object;
  release(list->tail); // Releases the tail 
  release(list->head); // Releases the head, which also releases the rest of the list
}

void lib_for_tests_link_destructor(obj *link)
{
  if (link) // Prevents us from decreasing list_size if the link is NULL
    {
      list_size--; // Decreases the list size by 1 when a link is destroyed
      //list->size--;
    }
  release(((new_link_t *) link)->next);
}

void lib_for_tests_list_negate()
{
  list_size--;
}

void lib_for_tests_size_reset()
{
  list_size = 0;
}

new_list_t *lib_for_tests_list_create()
{
  list = allocate(sizeof(new_list_t), lib_for_tests_destructor_linked_list);
  list->head = list->tail = NULL;
  list->size = 0;
  return list;
}

void lib_for_tests_linked_list_append()
{
  list_size++; // Easier to debug if it's incremented at the start
  list->size += 1;
  if(list->tail != NULL)
    {
      new_link_t *link = allocate(sizeof(new_link_t), lib_for_tests_link_destructor);
      link->next = NULL;
      release(list->tail);
      list->tail->next = link;
      retain(link);
      list->tail = link;
      retain(list->tail);
    }
  else
    {
      new_link_t *link = allocate(sizeof(new_link_t), lib_for_tests_link_destructor);
      link->next = NULL;
      list->head = link;
      retain(link);
      list->tail = link;
      retain(link);
    }
}

size_t lib_for_tests_linked_list_size()
{
  return list_size;
}


//----------Hash-table-------------//


/// Define the size of the hash table
#define Num_Buckets 17

static bucket_t *lib_for_tests_entry_create(int k, char *v, bucket_t *n)
{
  bucket_t *entry = allocate(sizeof(bucket_t), NULL);
  entry->key = k;
  entry->value = v;
  entry->next = n;
  return entry;
}

static bucket_t *lib_for_tests_find_previous_entry_for_key(bucket_t*bucket, int key)
{
  bucket_t*next_from_dummy = bucket;
  bucket_t*prev_next = bucket;
  while(bucket->next != NULL)
    {
      if(prev_next->next->key == key)
	{
	  return prev_next;
	}
      prev_next = bucket;
      bucket = bucket->next;
      next_from_dummy = bucket;
    }
  if(bucket->key == key)
    {
      return prev_next;
    }
  
  return next_from_dummy;
}

/// Deallocates the given entry. 
static void lib_for_tests_entry_destroy(bucket_t*entry)
{
  release(entry);     
}

hash_t *lib_for_tests_demo_hash_table_create()
{
hash_t *result = (hash_t *) allocate(sizeof(hash_t) + Num_Buckets*sizeof(bucket_t *), NULL);

  /// Create dummy nodes with key 0, value NULL and pointer NULL
  int i = 0;
  int key = 0;
  char *value = NULL;
  while(i < Num_Buckets)
    {
      result->buckets[i] = lib_for_tests_entry_create(key, value, NULL);
      retain(result->buckets[i]);
      ++i;
    }
  result->num_buckets = Num_Buckets;
  result->size = 0;
  return result;
}


void lib_for_tests_demo_hash_table_insert(hash_t *ht, int key, char *value)
{
  int bucket = key % ht->num_buckets;
  bucket_t*entry = lib_for_tests_find_previous_entry_for_key(ht->buckets[bucket], key);
  bucket_t*next = entry->next;

  if (next != NULL && next->key == key)
    {
      next->value = value;
    }
  else
    {
      entry->next = lib_for_tests_entry_create(key, value, next);
      retain(entry->next);
      ht->size++;
    }
}

void lib_for_tests_demo_hash_table_remove(hash_t *ht, int key)
{
  int bucket = key % ht->num_buckets;
  bucket_t*entry = lib_for_tests_find_previous_entry_for_key(ht->buckets[bucket], key);

  bucket_t*rm_entry = entry->next;
  if (rm_entry != NULL)
    {
      if(rm_entry->next != NULL)
	{
	  bucket_t*new_link = rm_entry->next;
	  entry->next = new_link;
	}
      else
	entry->next = NULL;
        

      lib_for_tests_entry_destroy(rm_entry);
      ht->size--;
      return;
    }
}

size_t lib_for_tests_demo_hash_table_size(hash_t *ht)
{
  size_t size = ht->size;
  return size;
}
