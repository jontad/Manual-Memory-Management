#include <stdlib.h>
#include <stdio.h>

#include "hash_table.h"
#include "linked_list.h"
#include "common.h"
#include "backend.h"
#include "frontend.h"
#include "utils.h"
#include <assert.h>

#include "../src/refmem.h"

//////////////////////////// CREATE/DESTROY DATABASE //////////////////////////////////

ioopm_database_t *database_create_database()
{
  ioopm_database_t *db = allocate(sizeof(ioopm_database_t), NULL);
  db->merch_ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_merch, 0.75, 17);
  db->shelves_ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_merch, 0.75, 17);
  db->carts = ioopm_hash_table_create(uns_int_hash_func, equality_function_uns_int, equality_function_pointer, 0.75, 17);
  db->id_counter = 0;

  retain(db->merch_ht);
  retain(db->merch_shelves_ht);
  retain(db->carts);
  return db;
}

static void free_items_in_cart(ioopm_link_t **element, void *extra)
{
  //free((*element)->value.ioopm_item);
  release((*element)->value.ioopm_item);
}

static void free_shelves_in_stock(ioopm_link_t **element, void *extra)
{
  //free((*element)->value.ioopm_shelf->shelf_name);
  //free((*element)->value.ioopm_shelf);
  release((*element)->value.ioopm_shelf->shelf_name);
  release((*element)->value.ioopm_shelf);
}

static void free_carts_apply_func(elem_t key, elem_t *value, void *extra)
{
  ioopm_linked_apply_to_all(value->ioopm_cart->basket, free_items_in_cart, NULL);
  ioopm_linked_list_destroy(value->ioopm_cart->basket);
  //free(value->ioopm_cart);
  release(value->ioopm_cart);
}

static void clear_stock(ioopm_list_t *stock)
{
  elem_t element;
  for (int i = 0; i < stock->list_size; ++i)
    {
      element = ioopm_linked_list_get(stock, i);
      //free(element.ioopm_shelf->shelf_name);
      //free(element.ioopm_shelf);
      release(element.ioopm_shelf->shelf_name);
      release(element.ioopm_shelf);
    }
}


static void destroy_merch(ioopm_database_t *db, merch_t *merch) //Note: remember to remove from hash table as well!
{
  ioopm_list_t *stock = merch->stock;
  ioopm_link_t *current_location = stock->first;
  
  while (current_location != NULL)  //1. remove from shelves_ht hash table
    {
      elem_t shelf = str_elem(current_location->value.ioopm_shelf->shelf_name);
      
      ioopm_hash_table_remove(db->shelves_ht, shelf);
      current_location = current_location->next;
    }
  clear_stock(stock);
  ioopm_linked_list_destroy(stock); //2. free list of shelves_ht
  //free(merch);
  release(merch);
}

static void free_merch_apply_func(elem_t key, elem_t *value, void *db)
{  
  destroy_merch(db, value->ioopm_merch);  
}

void database_destroy_database(ioopm_database_t *db)
{
  ioopm_hash_table_apply_to_all(db->carts, free_carts_apply_func, NULL);
  ioopm_hash_table_destroy(db->carts);
  ioopm_hash_table_apply_to_all(db->merch_ht, free_merch_apply_func, db);
  ioopm_hash_table_destroy(db->merch_ht);
  ioopm_hash_table_destroy(db->shelves_ht);

  //free(db);
  release(db);
}



/////////////////////////////////// ADD /////////////////////////////////////////////////
merch_t *database_add_merch(ioopm_database_t *db, char *new_name, char *new_desc, unsigned int new_price) 
{  
  merch_t *merch = allocate(sizeof(merch_t), NULL);
  merch->name = new_name;
  merch->desc = new_desc;
  merch->price_per_unit = new_price;
  merch->available_amount = 0;
  merch->stock = ioopm_linked_list_create(equality_function_str);

  ioopm_hash_table_insert(db->merch_ht, str_elem(merch->name), merch_elem(merch));
  retain(merch->stock);
  return merch;
}


////////////////////////////////////// LIST /////////////////////////////////////////////


static int strcompare(const void *merch1, const void *merch2)
{
  return strcmp(*(char * const *) merch1, *(char * const *) merch2);
}


char **database_sort_list(ioopm_list_t *list) //returns a sorted list of merch names
{
  ioopm_link_t *current_link = list->first;
  retain(current_link);
  
  size_t size = ioopm_linked_list_size(list);
  char **merch_list = allocate_array(size, sizeof(char *), NULL);
  
  for(int i = 0; i < size; ++i)
    {
      merch_list[i] = current_link->value.ioopm_str;
      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }

  qsort(merch_list, size, sizeof(char *), strcompare);

  return merch_list;
}




option_t database_choose_merch(ioopm_database_t *db, int result)
{
  ioopm_list_t *names = ioopm_hash_table_keys(db->merch_ht); // generate list.
  retain(names);
  
  char **merch_list = database_sort_list(names);
  retain (merch_list);
  
  char *chosen_merch = merch_list[result-1]; //choose value from sorted list
  option_t merch = ioopm_hash_table_lookup(db->merch_ht, str_elem(chosen_merch));

  assert(Successful(merch) && "Merch not found");
  
  //free(merch_list);
  release(merch_list);
  ioopm_linked_list_destroy(names);
  return merch;
}


///////////////////////////////// REMOVE ////////////////////////////////////////////

void database_remove_merch(ioopm_database_t *db, merch_t *merch)
{ 
  ioopm_hash_table_remove(db->merch_ht, str_elem(merch->name));      
  destroy_merch(db, merch);
}


///////////////////////////////// EDIT ///////////////////////////////////////////


static merch_t *rename_merch(char *name, merch_t *merch)
{
  merch_t *new_merch = allocate(sizeof(merch_t), NULL);
  new_merch->name = name; 
  new_merch->desc = merch->desc;
  new_merch->price_per_unit = merch->price_per_unit;
  new_merch->available_amount = merch->available_amount;
  new_merch->stock = merch->stock;

  retain(new_merch->stock);
  return new_merch; 
}


static merch_t *insert_merch(ioopm_database_t *db, char *new_name, merch_t *merch)
{
  merch_t *new_merch = rename_merch(new_name, merch);
  retain(new_merch);
  
  ioopm_hash_table_remove(db->merch_ht, str_elem(merch->name));
  
  ioopm_linked_apply_to_all(merch->stock, free_shelves_in_stock, NULL);
  //free(merch);
  release(merch);
  ioopm_hash_table_insert(db->merch_ht, str_elem(new_name), merch_elem(new_merch));    
  return new_merch;
}

merch_t *database_edit_name(ioopm_database_t *db, char *new_name, merch_t *merch)
{
  merch = insert_merch(db, new_name, merch);
  return merch;
}

void database_edit_desc(merch_t *merch, char *new_desc)
{
  merch->desc = new_desc;
}


void database_edit_price(merch_t *merch, int new_price)
{
  merch->price_per_unit = new_price;
}

///////////////////////////////// SHOW STOCK /////////////////////////////////////
void database_show_stock(merch_t *merch)
{
  ioopm_link_t *current_link = merch->stock->first;
  retain(current_link);
  
  while (current_link != NULL)
    {
      char *shelf = current_link->value.ioopm_shelf->shelf_name;
      int amount = current_link->value.ioopm_shelf->amount;	

      printf("\nShelf: %s, Amount: %d", shelf, amount); 

      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
  printf("\n");
}

////////////////////////////////// REPLENISH //////////////////////////////////////////////

shelf_t *database_create_shelf(char *shelf_name, int amount)
{
  shelf_t *new_shelf = allocate(sizeof(shelf_t), NULL);
  new_shelf->shelf_name = shelf_name;
  new_shelf->amount = amount;

  return new_shelf;
}

static int shelf_value(char *shelf_name)
{
  int value1 = shelf_name[0];
  int value2 = shelf_name[1];
  int value3 = shelf_name[2];

  return (value1*100 + value2*10 + value3);
}

static int shelf_compare(ioopm_list_t *stock, char *shelf_name)
{
  int counter = 0;
  ioopm_link_t *current_loc = stock->first;
  retain(current_loc);
  
  if (current_loc != NULL)
    {
      int current_shelf_value = shelf_value(current_loc->value.ioopm_shelf->shelf_name);
      int new_shelf_value = shelf_value(shelf_name);
  
      while(current_loc != NULL && new_shelf_value > current_shelf_value)
	{
	  current_shelf_value = shelf_value(current_loc->value.ioopm_merch->name);
	  ++counter;
	  
	  release(current_loc);
	  current_loc = current_loc->next;
	  retain(current_loc);

	}
    }
  return counter;
}

  
void database_replenish_stock(ioopm_database_t *db, merch_t *merch, shelf_t *new_shelf)
{
  merch->available_amount = merch->available_amount + new_shelf->amount;

  bool replenish_existing_shelf = false;
  ioopm_link_t *current_link = merch->stock->first;
  retain(current_link);
  
  shelf_t *current_shelf;
  
  while(current_link != NULL && !replenish_existing_shelf)
    {
      current_shelf = current_link->value.ioopm_shelf;
      
      if (strcmp(current_shelf->shelf_name, new_shelf->shelf_name) == 0)
	{
	  replenish_existing_shelf = true;
	  break;
	}

      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
  
  if(replenish_existing_shelf)
    {
      current_link->value.ioopm_shelf->amount += new_shelf->amount;
    }
  else
    {
      int index = shelf_compare(merch->stock, new_shelf->shelf_name);  //Index shelves in alphanumerical order
      ioopm_linked_list_insert(merch->stock, index, shelf_elem(new_shelf));
      ioopm_hash_table_insert(db->shelves_ht, str_elem(new_shelf->shelf_name), merch_elem(merch));
    }
}

/////////////////////////////// CREATE CART ////////////////////////////////

cart_t *database_create_cart(ioopm_database_t *db)
{
  cart_t *cart = allocate(sizeof(cart_t), NULL);
  cart->id = db->id_counter;
  ++db->id_counter;
  cart->basket = ioopm_linked_list_create(equality_function_merch);
  retain(cart->basket);
  ioopm_hash_table_insert(db->carts, int_elem(cart->id), cart_elem(cart));
  return cart;
}

//////////////////////////////// REMOVE CART ////////////////////////////////// 


void database_delete_cart(ioopm_database_t *db, cart_t *cart)
{
  ioopm_linked_apply_to_all(cart->basket, free_items_in_cart, NULL);
  ioopm_hash_table_remove(db->carts, unsigned_elem(cart->id));
  ioopm_linked_list_destroy(cart->basket);
  //free(cart);
  release(cart);
}

void database_remove_cart(ioopm_database_t *db, cart_t *cart)
{
  database_delete_cart(db, cart);
}

///////////////////////////// ADD TO CART //////////////////////////////////

static item_t *item_create(merch_t *merch, int amount)
{
  item_t *item = allocate(sizeof(item_t), NULL);
  item->name = merch->name;
  item->amount = amount;
  item->price_per_unit = merch->price_per_unit;
  item->pointer = merch;
  retain(item->pointer);
  return item;
} 

void database_add_to_cart(ioopm_database_t *db, cart_t *cart, merch_t *merch, int amount)
{
  item_t *item = item_create(merch, amount);

  merch->available_amount = merch->available_amount - amount;

  ioopm_linked_list_insert(cart->basket, 0, item_elem(item));
}

    

/////////////////////////// REMOVE FROM CART //////////////////////////////////

option_t database_choose_item_in_cart(cart_t *cart, char *wanted_item) 
{
  ioopm_link_t *current_link = cart->basket->first;
  retain(current_link);
  if(current_link == NULL)
    {
      release(current_link);
      return Failure();
    }
   
  char *current_item = current_link->value.ioopm_item->name;
    
  while(current_link != NULL && strcmp(current_item, wanted_item) != 0)
    {
      current_item = current_link->value.ioopm_item->name; 
      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
   
  if(strcmp(current_item, wanted_item) == 0)
    {
      elem_t item = current_link->value;
      return Success(item);
    }
  else
    {
      printf("\n%s not in cart.\n", wanted_item);
      return Failure();
    }   
}


void database_remove_from_cart(cart_t *cart, item_t *item, int amount)
{
  merch_t *merch = item->pointer;
  retain(merch);
  
  ioopm_link_t *current_link = cart->basket->first;
  retain(current_link);
  
  int index = 0;
  while(current_link != NULL && current_link->value.ioopm_item->name != merch->name) 
    {
      release(current_link);
      current_link = current_link->next;
      retain(current_link);
      ++index;
    }
  if (current_link != NULL) //If current_link reaches NULL, the item is not in the cart
    {
      item->amount = item->amount - amount;
      
      if(item->amount == 0)
	{
	  ioopm_linked_list_remove(cart->basket, index); 
	  //free(item);
	  release(item);
	}
      merch->available_amount = merch->available_amount + amount;
    }
  else
    {
      printf("%s not in cart %d.\n", merch->name, cart->id);
    }
}

//////////////////////////// CALCULATE COST ///////////////////////////////////
    
static unsigned int total_price(cart_t *cart)
{
  unsigned int total = 0;
  ioopm_link_t *current_link = cart->basket->first;
  retain(current_link);
  while (current_link != NULL)
    {
      int amount = current_link->value.ioopm_item->amount;
      int price_per_unit = current_link->value.ioopm_item->price_per_unit;
      
      total = total + (amount * price_per_unit);

      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
  return total;
}

void database_print_cart(cart_t *cart)
{
  char *name = NULL;
  unsigned int amount = 0;
  unsigned int price = 0;
  unsigned int total = total_price(cart);
  
  printf("\nCart ID: %u\n", cart->id);

  ioopm_link_t *current_link = cart->basket->first;
  retain(current_link);
  while(current_link != NULL)
    {
      name = current_link->value.ioopm_item->name;
      amount = current_link->value.ioopm_item->amount;
      price = current_link->value.ioopm_item->price_per_unit;

      printf("%s: %u x %u SEK\n", name, amount, price);

      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
  printf("Total price: %u SEK\n\n", total);

}

////////////////////////////// CHECKOUT ///////////////////////////////////////
bool database_items_in_cart_exist(ioopm_database_t *db, cart_t *cart)
{
  bool exists = true;
  for(int i = 0; i < cart->basket->list_size; ++i)
    {
      elem_t item = ioopm_linked_list_get(cart->basket, i);
      option_t result = ioopm_hash_table_lookup(db->merch_ht, str_elem(item.ioopm_item->name));
      if(Unsuccessful(result))
	{
	  exists = false;
	  return exists;
	}
    }
  return exists;
}

static void remove_from_stock(ioopm_link_t *current_link)
{
  ioopm_link_t *current_shelf = current_link->value.ioopm_item->pointer->stock->first;
  retain(current_shelf);
  
  while(current_shelf->value.ioopm_shelf->amount == 0)
    {
      release(current_shelf);
      current_shelf = current_shelf->next;
      retain(current_shelf);
    }
  --current_link->value.ioopm_item->amount;
  --current_link->value.ioopm_item->pointer->stock->first->value.ioopm_shelf->amount;
}

void database_checkout(ioopm_database_t *db, cart_t *cart)
{  
  ioopm_link_t *current_link = cart->basket->first;     
  retain(current_link);
  
  while(current_link != NULL)
    {
      while(current_link->value.ioopm_item->amount > 0)
	{
	  remove_from_stock(current_link);
	}
      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
   database_delete_cart(db, cart);
}

