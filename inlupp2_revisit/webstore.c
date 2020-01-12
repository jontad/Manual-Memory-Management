#include <stdlib.h>
#include <stdio.h>

#include "hash_table.h"
#include "linked_list.h"
#include "common.h"
#include "webstore.h"
#include "utils.h"

#define Success(v)      (option_t) { .success = true, .value = v };
#define Failure()       (option_t) { .success = false };
#define Successful(o)   (o.success == true)
#define Unsuccessful(o) (o.success == false)

#define int_elem(i) (elem_t) {.ioopm_int = (i)}
#define unsigned_elem(u) (elem_t) {.ioopm_u_int = (u)}
#define bool_elem(b) (elem_t) {.ioopm_bool = (b)}
#define str_elem(s) (elem_t) {.ioopm_str = (s)}
#define float_elem(f) (elem_t) {.ioopm_float = (f)}
#define void_elem(v) (elem_t) {.ioopm_void_ptr = (v)}
#define merch_elem(m) (elem_t) {.ioopm_merch = (m)}
#define shelf_elem(sh) (elem_t) {.ioopm_shelf = (sh)}
#define cart_elem(c) (elem_t) {.ioopm_cart = (c)}
#define item_elem(ci) (elem_t) {.ioopm_item = (ci)}

//////////////////////////////// STRUCTS /////////////////////////////////////



struct database
{
  ioopm_hash_table_t *merch_ht;
  ioopm_hash_table_t *shelves_ht;
  ioopm_hash_table_t *carts;
  unsigned int id_counter; 
};


struct option
{
  bool success;
  elem_t value; 
};


struct link
{
  elem_t value;
  ioopm_link_t *next;
};

struct list
{
  size_t list_size; 
  ioopm_eq_function equal;
  ioopm_link_t *first;
  ioopm_link_t *last;
};


struct hash_table
{
  float load_factor;
  size_t capacity;
  entry_t **buckets;
  hash_function hash_func;
  ioopm_eq_function key_eq_func;
  ioopm_eq_function value_eq_func;
  size_t size;
};





ioopm_database_t *create_database()
{
  ioopm_database_t *db = calloc(1, sizeof(ioopm_database_t));
  db->merch_ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_merch, 0.75, 17);
  db->shelves_ht = ioopm_hash_table_create(str_hash_func, equality_function_str, equality_function_merch, 0.75, 17);
  db->id_counter = 0;
  return db;
}

/////////////////////////////////// ADD /////////////////////////////////////////////////

static merch_t *create_merch()
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = ask_question_string("Name of merchandise: ");  
  merch->desc = ask_question_string("Description of merchandise: ");  
  merch->price_per_unit = ask_question_int("Price of merchandise: ");
  merch->available_amount = 0;
  merch->stock = ioopm_linked_list_create(equality_function_str);
  return merch; 
}

//TODO: Hjälpfunktion för att uppdatera value (pekare)  av location och merch samtidigt
void ioopm_add_merch(ioopm_database_t *db) 
{
  merch_t *new_merch = create_merch();
  option_t lookup_merch = ioopm_hash_table_lookup(db->merch_ht, str_elem(new_merch->name));

  if (Unsuccessful(lookup_merch))
    {
      ioopm_hash_table_insert(db->merch_ht, str_elem(new_merch->name), merch_elem(new_merch)); 
    }
  else
    {
      printf("%s already exists!", new_merch->name);
    }
}


////////////////////////////////////// LIST /////////////////////////////////////////////

static bool ask_continue_listing()
{
  char *answer;
  bool cont;
  bool quit;

  do //asks to continue listing merchandise after printing 20 items
    {
      answer = ask_question_string("[C]ontinue listing items, or [Q]uit: ");

      cont = (toupper(answer[0]) == 'C' && (strlen(answer) == 1));
      quit = (toupper(answer[0]) == 'Q' && (strlen(answer) == 1));
    }
  while (!cont && !quit);
  
  return quit;
}



static int strcompare(const void *merch1, const void *merch2)
{
  return strcmp(*(char * const *) merch1, *(char * const *) merch2);
}


static char **sort_list(ioopm_list_t *list) //returns a sorted list of merch names
{
  ioopm_link_t *current_link = list->first;
  
  size_t size = ioopm_linked_list_size(list);
  char **merch_list = calloc(size, sizeof(char *));
  
  for(int i = 0; i < size; ++i)
    {
      merch_list[i] = current_link->value.ioopm_str;
      current_link = current_link->next;
    }

  qsort(merch_list, size, sizeof(char *), strcompare);

  return merch_list;
}


void ioopm_list_merch(ioopm_database_t *db)
 {
   ioopm_list_t *names = ioopm_hash_table_keys(db->merch_ht); // generate list
   size_t size = ioopm_linked_list_size(names);
   char **merch_list = sort_list(names);             // sort list

   if(size == 0)
     {
       puts("\nNo merchandise in database!");
     }
   
   for (int i = 0; i < size; ++i)
     {
       if (((i+1) % 21) == 0)
	 {
	   bool quit = ask_continue_listing();
	   if (quit) break;
	 }
       printf("%d. %s\n", i+1, merch_list[i]); //i+1: remember, arrays start a 0
     }
   printf("\n");
 }

static merch_t *choose_merch(ioopm_database_t *db) //choose merch from list with number
{  
  int result;
  ioopm_list_merch(db);
  do
    {
      result = ask_question_int("Choose merchandise by number: ");
    }
  while((1 > result) || (result > db->merch_ht->size));


  ioopm_list_t *names = ioopm_hash_table_keys(db->merch_ht); // generate list.
   
  char **merch_list = sort_list(names);
  char *chosen_merch = merch_list[result-1]; //choose value from sorted list

  option_t merch_opt = ioopm_hash_table_lookup(db->merch_ht, str_elem(chosen_merch));
  merch_t *merch = merch_opt.value.ioopm_merch;

  return merch;
}

///////////////////////////////// REMOVE ////////////////////////////////////////////

static void destroy_merch(ioopm_database_t *db, ioopm_list_t *list,  ioopm_link_t *current_location, elem_t name)
{
  while (current_location != NULL)  //1. remove from shelves_ht hash table
    {
      elem_t shelf = str_elem(current_location->value.ioopm_shelf->shelf_name);
      
      ioopm_hash_table_remove(db->shelves_ht, shelf);
      current_location = current_location->next;
    }
      
  ioopm_linked_list_destroy(list); //2. free list of shelves_ht
  ioopm_hash_table_remove(db->merch_ht, name); //3. remove from merch hash table
}



void ioopm_remove_merch(ioopm_database_t *db)
{ 
  merch_t *merch = choose_merch(db);
  
  ioopm_list_t *list = merch->stock;
  elem_t name = str_elem(merch->name);
  ioopm_link_t *current_location = list->first;
      
  destroy_merch(db, list, current_location, name);

}


///////////////////////////////// EDIT ///////////////////////////////////////////
static merch_t *rename_merch(char *name, merch_t *merch)
{
  merch_t *new_merch = calloc(1, sizeof(merch_t));
  new_merch->name = name; 
  new_merch->desc = merch->desc;
  new_merch->price_per_unit = merch->price_per_unit;
  new_merch->available_amount = merch->available_amount;
  new_merch->stock = merch->stock;
  return new_merch; 
}


void insert_merch(ioopm_database_t *db, char *new_name, merch_t *merch)
{
  merch_t *new_merch = rename_merch(new_name, merch);

  ioopm_hash_table_remove(db->merch_ht, str_elem(merch->name));
  ioopm_hash_table_insert(db->merch_ht, str_elem(new_name), merch_elem(new_merch));    
}


void print_item(char *name, char *desc, int price)
{
  printf("\nName: %s\n", name);
  printf("Description: %s\n", desc);
  printf("Price: %d\n\n", price);
}

void event_loop_edit(ioopm_database_t *db, merch_t *merch)
{
  char *answer;
  char *new_name; 
  do
    {
      answer = ask_question_string("Edit [n]ame\nEdit [d]escription\nEdit [p]rice\n[Q]uit\n");
      if(strlen(answer) == 1)
	{
	  if(toupper(answer[0]) == 'N')
	    {
	      new_name = ask_question_string("New name of merchandise: ");
	      insert_merch(db, new_name, merch);
	      print_item(new_name, merch->desc, merch->price_per_unit);
	    }
	  if(toupper(answer[0]) == 'D')
	    {
	      merch->desc = ask_question_string("New description of merchandise: ");
	      print_item(merch->name, merch->desc, merch->price_per_unit);
	    }
	  if(toupper(answer[0]) == 'P')
	    {
	      merch->price_per_unit = ask_question_int("New price of merchandise: ");
	      print_item(merch->name, merch->desc, merch->price_per_unit);
	    }
	}
    }
  while(toupper(answer[0]) != 'Q' || strlen(answer) != 1);
      
}

void ioopm_edit_merch(ioopm_database_t *db)
{
  merch_t *merch =  choose_merch(db);

  print_item(merch->name, merch->desc, merch->price_per_unit);
  event_loop_edit(db, merch);
    
   
}


///////////////////////////////// SHOW STOCK /////////////////////////////////////    //STILL SEG FAULTS YO

void ioopm_show_stock(ioopm_database_t *db)
{
  merch_t *merch = choose_merch(db);
  
  ioopm_link_t *current_link = merch->stock->first;
  while (current_link != NULL)
    {
      char *shelf = current_link->value.ioopm_shelf->shelf_name;
      int amount = current_link->value.ioopm_shelf->amount;
	
      printf("Shelf: %s, Amount: %d\n", shelf, amount); 
      current_link = current_link->next;
    }
    

}

////////////////////////////////// REPLENISH //////////////////////////////////////////////

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
  if (current_loc != NULL)
    {
      int current_shelf_value = shelf_value(current_loc->value.ioopm_merch->name);
      int new_shelf_value = shelf_value(shelf_name);
  
      while(current_loc != NULL && new_shelf_value < current_shelf_value)
	{
	  ++counter;
	  current_loc = stock->first->next;
	  current_shelf_value = shelf_value(current_loc->value.ioopm_merch->name);
	}
    }
  return counter;
}


void ioopm_replenish_stock(ioopm_database_t *db)
{      
  merch_t *merch = choose_merch(db);
 
  shelf_t new_shelf;
  new_shelf.shelf_name = ask_question_shelf("Storage location: ");
  new_shelf.amount = ask_question_int("Amount: ");

  merch->available_amount = merch->available_amount + new_shelf.amount;
      
  int index = shelf_compare(merch->stock, new_shelf.shelf_name);  //Index shelves in alphanumerical order
  
  ioopm_linked_list_insert(merch->stock, index, str_elem(new_shelf.shelf_name));
  ioopm_hash_table_insert(db->shelves_ht, str_elem(new_shelf.shelf_name), merch_elem(merch));
}

/////////////////////////////// CREATE CART ////////////////////////////////

cart_t *ioopm_create_cart(ioopm_database_t *db)
{
  cart_t *cart = calloc(1, sizeof(cart_t));
  cart->id = db->id_counter;
  ++db->id_counter;
  cart->total_price = 0;
  cart->basket = ioopm_linked_list_create(equality_function_merch);
  ioopm_hash_table_insert(db->carts, int_elem(cart->id), cart_elem(cart));
  return cart;
}


//////////////////////////////// REMOVE CART //////////////////////////////////
void ioopm_remove_cart(ioopm_database_t *db, cart_t *cart) //TODO: ask for cart
{
  ioopm_linked_list_destroy(cart->basket);
  ioopm_hash_table_remove(db->carts, unsigned_elem(cart->id));
  free(cart);
}

///////////////////////////// ADD TO CART //////////////////////////////////

static cart_t *choose_cart(ioopm_database_t *db)
{
  int result;
  option_t cart;
  do
    {
      result = ask_question_int("Cart ID: ");
      cart = ioopm_hash_table_lookup(db->carts, int_elem(result));
    }
  while((result < 0) || (result > db->id_counter) || Unsuccessful(cart));
  return cart.value.ioopm_cart;
}

static item_t *item_create(merch_t *merch, int amount)
{
  item_t *item = calloc(1, sizeof(item_t));
  item->amount = amount;
  item->price_per_unit = (merch->price_per_unit) * amount;
  item->pointer = merch;
  return item;
} //TODO: dubbelkolla att pointer är en pekare

void ioopm_add_to_cart(ioopm_database_t *db)
{
  merch_t *merch = choose_merch(db);
  int amount;

  //list_id(); MAYBE
  
  cart_t *cart = choose_cart(db);
  do
    {
      amount = ask_question_int("Amount: ");
    }
  while((amount < 0) || (merch->available_amount < amount));

  merch->available_amount = merch->available_amount - amount;

  
  item_t *item = item_create(merch, amount);
  
  ioopm_linked_list_insert(cart->basket, 0, item_elem(item));
	
}

/////////////////////////// REMOVE FROM CART //////////////////////////////////

void ioopm_remove_from_cart(ioopm_database_t *db)
{
  cart_t *cart = choose_cart(db);
  merch_t *merch = choose_merch(db);

  ioopm_link_t *current_link = cart->basket->first;
  int index = 0;
  while(current_link->value.ioopm_item->pointer->name != merch->name && current_link != NULL)
    {
      current_link = current_link->next;
      ++index;
    }

  if(current_link != NULL)
    {
      item_t *item = current_link->value.ioopm_item;
  
      int amount;
      do
	{
	  amount = ask_question_int("Amount: ");
	}
      while((amount < 1) || ( amount > item->amount));

      item->amount = item->amount - amount;
 
      if(item->amount == 0)
	{
	  ioopm_linked_list_remove(cart->basket, index);
	}

      merch->available_amount = merch->available_amount + amount;
    }
  else
    {
      printf("%s not in cart %d.\n", merch->name, cart->id);
    } 
}











//////////////////////////// CALCULATE COST ///////////////////////////////////
unsigned int ioopm_calculate_cost(cart_t *cart)
{
  return cart->total_price;
}

/////////////////////////////////EVENT_LOOP/////////////////////////////////


int event_loop(ioopm_database_t *db)
{
  char answer;
  do
    {
      answer = ask_question_menu(""); //Placeholer
      if (answer == 'A') //Add merch
	{
	  ioopm_add_merch(db); 
	}
      if (answer == 'L') //List merch
	{
	  ioopm_list_merch(db);
	}
      if (answer == 'R') //Remove merch
	{
	  ioopm_remove_merch(db);
	}
      if (answer == 'E') //Edit merch
	{
	  ioopm_edit_merch(db);
	}
      if (answer == 'S') //Show stock
	{
	  ioopm_show_stock(db);
	}
      if (answer == 'P')//Replenish stock
	{
	  ioopm_replenish_stock(db);
	}
      if (answer == 'C') //Create cart
	{
	  printf("Not yet implemented!\n\n");
	}
      if (answer == 'M') //Remove cart
	{
	  printf("Not yet implemented!\n\n");
	}
      if (answer == 'T') //Add to cart
	{
	  printf("Not yet implemented!\n\n");
	}
      if (answer == 'F') //Remove from cart
	{
	  printf("Not yet implemented!\n\n");
	}
      if (answer == 'O') //Calculate cost
	{
	  printf("Not yet implemented!\n\n");
	}
      if (answer == 'H') //Checkout
	{
	  printf("Not yet implemented!\n\n");
	}
      if (answer == 'U') //Undo
	{
	  printf("Not yet implemented!\n\n");
	}
    }
  while (answer != 'Q');
  return 0;
}



///////////////////////////////MAIN/////////////////////////////////
int main()
{
  ioopm_database_t *db = create_database();
  event_loop(db);
}
