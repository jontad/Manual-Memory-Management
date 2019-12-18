#include <stdio.h>

#include "hash_table.h"
#include "linked_list.h"

#include "backend.h"
#include "common.h"
#include "utils.h"

#include "../src/refmem.h" 

struct option
{
  bool success;
  elem_t value; 
};


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


///////////////////////////// STRUCTS ///////////////////////////////


struct database
{
  ioopm_hash_table_t *merch_ht;
  ioopm_hash_table_t *shelves_ht;
  ioopm_hash_table_t *carts;
  unsigned int id_counter; 
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

struct item
{
  char *name;
  unsigned int amount; // amount of merch in cart
  unsigned int price_per_unit;
  merch_t *pointer; //pointer to merch in ht
};

struct cart
{
  unsigned int id; // key 
  ioopm_list_t *basket;                                     
};

struct shelf
{
  char *shelf_name;
  unsigned int amount; //amount on shelf
};

struct merch
{
  char *name; //key
  char *desc;
  unsigned int price_per_unit;
  unsigned int available_amount; //amount of merch in stock that is not in cart
  ioopm_list_t *stock;
};


///////////////////////////////// MISC ////////////////////////////////////////

bool ioopm_ask_to_continue(char *question)
{
  char *answer = NULL;
  bool cont = false;
  bool quit = false;

  do //asks to continue listing merchandise after printing 20 items
    {
      //free(answer);
      release(answer);
      answer = ask_question_string(question);

      cont = (toupper(answer[0]) == 'Y' && (strlen(answer) == 1));
      quit = (toupper(answer[0]) == 'N' && (strlen(answer) == 1));
    }
  while (!cont && !quit);

  //free(answer);
  release(answer);
  return !quit;
}






static void print_item(char *name, char *desc, int price)
{
  printf("\nName: %s\n", name);
  printf("Description: %s\n", desc);
  printf("Price: %d\n\n", price);
}


/////////////////////////////////// ADD /////////////////////////////////////////////////


void ioopm_add_merch(ioopm_database_t *db)
{

  char *name = ask_question_string("Name of merchandise: ");
  option_t result = ioopm_hash_table_lookup(db->merch_ht, str_elem(name)); //Check if a merch of that name already exists
  
  if (!result.success)
    {
      char *desc = ask_question_string("Description of merchandise: ");
      int price = ask_question_int("Price of merchandise: ");

      if(price >= 0)
	{
	  database_add_merch(db, name, desc, price);
	}
      else
	{
	  puts("Price must be a positve number!");
	}
    }
  else
    {
      printf("%s already exists!\n", name);
      //free(name);
      release(name);
    }
}

/////////////////////////////////// LIST ///////////////////////////////////////


void ioopm_list_merch(ioopm_database_t *db)
 {
   ioopm_list_t *names = ioopm_hash_table_keys(db->merch_ht); // generate list
   size_t size = ioopm_linked_list_size(names);
   char **merch_list = database_sort_list(names);             // sort list

   if(size == 0)
     {
       puts("\nNo merchandise in database!");
     }
   
   for (int i = 0; i < size; ++i)
     {
       if (((i+1) % 3) == 0) 
	 {
	   bool cont = ioopm_ask_to_continue("Continue listing? [Y/N]");
	   if (!cont) break;
	 }
       printf("%d. %s\n", i+1, merch_list[i]); //i+1: remember, arrays start a 0
     }
   printf("\n");
   //free(merch_list);
   release(merch_list);
   ioopm_linked_list_destroy(names);
 }


option_t choose_merch(ioopm_database_t *db) //choose merch from list with number
{  
  ioopm_list_merch(db);

  int number = 0;
  if(db->merch_ht->size != 0)
    {
      do
	{
	  number = ask_question_int("Choose merchandise by number: ");
	}
      while((1 > number) || (number > db->merch_ht->size));
      
      option_t merch = database_choose_merch(db, number);
      return merch;
    }
  return Failure();
}


///////////////////////////////// REMOVE ////////////////////////////////////////////

void ioopm_remove_merch(ioopm_database_t *db)
{
  option_t result = choose_merch(db);

  if(Successful(result))
    {
      bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
      if (!cont) return;
      
      merch_t *merch = result.value.ioopm_merch;
      database_remove_merch(db, merch);
    }
}


///////////////////////////////// EDIT ///////////////////////////////////////////

void event_loop_edit(ioopm_database_t *db, merch_t *merch)
{
  char *answer = NULL;
  char *new_name = NULL;
  option_t lookup_result;
  do
    {
      //free(answer);
      release(answer);
      answer = ask_question_string("Edit [n]ame\nEdit [d]escription\nEdit [p]rice\n[Q]uit\n");
      if(strlen(answer) == 1)
	{
	  if(toupper(answer[0]) == 'N')
	    {
	      new_name = ask_question_string("New name of merchandise: ");
	      lookup_result = ioopm_hash_table_lookup(db->merch_ht, str_elem(new_name));

	      bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
	      if (cont)
		{
		  if (Unsuccessful(lookup_result))
		    {
		      //free(merch->name);
		      release(merch->name);
		      merch = database_edit_name(db, new_name, merch);
		      print_item(merch->name, merch->desc, merch->price_per_unit);

		    }
		  else
		    {
		      printf("%s already exists!\n", new_name);
		      //free(new_name);
		      release(new_name);
		    }
		}
	      else
		{
		  //free(new_name);
		  release(new_name);
		}
	    }
	  if(toupper(answer[0]) == 'D')
	    {
	      char *new_desc = ask_question_string("New description of merchandise: ");	    
	      bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
	      if (cont)
		{
		  //free(merch->desc);
		  release(merch->desc);
		  database_edit_desc(merch, new_desc);
		  print_item(merch->name, merch->desc, merch->price_per_unit);
		}
	      else
		{
		  //free(new_desc);
		  release(new_desc);
		  print_item(merch->name, merch->desc, merch->price_per_unit);
		}
	    }
	  if(toupper(answer[0]) == 'P')
	    {
	      int new_price = ask_question_int("New price of merchandise: ");
	      bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
	      if (cont)
		{
		  database_edit_price(merch, new_price);
		}
	    }
	}
    }
  while(toupper(answer[0]) != 'Q' || strlen(answer) != 1);

  //free(answer);
  release(answer);    
}

void ioopm_edit_merch(ioopm_database_t *db)
{
  option_t result = choose_merch(db);
  if(Successful(result))
    {
      merch_t *merch = result.value.ioopm_merch;
      print_item(merch->name, merch->desc, merch->price_per_unit);
      event_loop_edit(db, merch); 
    }
}

///////////////////////////// SHOW STOCK ////////////////////////////////////////////



void ioopm_show_stock(ioopm_database_t *db)
{
  option_t result = choose_merch(db);
  if(Successful(result))
    {
      merch_t *merch = result.value.ioopm_merch;
      database_show_stock(merch);
    }
}




///////////////////////////// REPLENISH ////////////////////////////////////////////


void ioopm_replenish_stock(ioopm_database_t *db)
{
  option_t result = choose_merch(db);
  if(Successful(result))
    {
      merch_t *merch = result.value.ioopm_merch;
      
      char *shelf_name = ask_question_shelf("Storage location: ");
      int amount = ask_question_int("Amount: ");

      shelf_t *new_shelf = database_create_shelf(shelf_name, amount);
     
      database_replenish_stock(db, merch, new_shelf);
    }
}


/////////////////////////////// CREATE CART ///////////////////////////////


void ioopm_create_cart(ioopm_database_t *db)
{
  cart_t *cart = database_create_cart(db);
  printf("\nCart ID: %u\n", cart->id);
}


/////////////////////////////// REMOVE CART ////////////////////////////////


//Note: array_from_list_id and list_id are modified versions of ioopm_list_merch
 static unsigned int *array_from_list_id(ioopm_list_t *list) //returns a sorted list of id
{
  ioopm_link_t *current_link = list->first;
  
  size_t size = ioopm_linked_list_size(list);
  
  unsigned int *id_list = calloc(size, sizeof(unsigned int));
  
  for(int i = 0; i < size; ++i)
    {
      id_list[i] = current_link->value.ioopm_u_int;
      current_link = current_link->next;
    }
  return id_list;
}


static void list_id(ioopm_database_t *db)
 {
   ioopm_list_t *id = ioopm_hash_table_keys(db->carts); // generate list
   size_t size = ioopm_linked_list_size(id);
   unsigned int *id_list = array_from_list_id(id);

   if(size == 0)
     {
       puts("\nNo carts in database!");
     }
   
   for (int i = 0; i < size; ++i)
     {
       if (((i+1) % 21) == 0)
	 {
	   bool cont = ioopm_ask_to_continue("Continue listing? [Y/N]");
	   if (!cont) break;
	 }
       printf("Cart ID: %u\n", id_list[i]);
     }
   printf("\n");
   ioopm_linked_list_destroy(id);
   //free(id_list);
   release(id_list);
 }



static option_t choose_cart(ioopm_database_t *db)
{
  list_id(db);
    
  if(db->carts->size != 0)
    {
      int result = 0;
      option_t cart;
      
      do
	{
	  result = ask_question_int("Choose cart by cart ID: ");
	  cart = ioopm_hash_table_lookup(db->carts, int_elem(result));
      
	}
      while((result < 0) || (result > db->id_counter) || Unsuccessful(cart));
      return cart;
    }
  
  return Failure();
}

void ioopm_remove_cart(ioopm_database_t *db)
{
  option_t result = choose_cart(db);
  if (Successful(result))
    {
      bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
      if (!cont) return;
      cart_t *cart = result.value.ioopm_cart;
      database_remove_cart(db, cart);
    }
}

///////////////////////////// ADD TO CART //////////////////////////////////////

static int get_amount_merch(merch_t *merch)
{
  int amount = 0;
  do
    {
      amount = ask_question_int("Amount: ");
    }
  while((amount < 0) || (merch->available_amount < amount));
  
  return amount;
}

void ioopm_add_to_cart(ioopm_database_t *db)
{
  option_t result_carts = choose_cart(db); //Choose cart
   if (Successful(result_carts))
     {
       cart_t *cart = result_carts.value.ioopm_cart;
       
       option_t result_merch = choose_merch(db); //Choose merch
       merch_t *merch = result_merch.value.ioopm_merch;
      
       if(Successful(result_merch))
	 {      
	   if(merch->available_amount != 0) //Check if merch is in stock
	     {

	       printf("Available amount: %d\n", merch->available_amount);
	       int amount = get_amount_merch(merch); //Choose amount to add
	       	       
	       database_add_to_cart(db, cart, merch, amount);
	     }
	   else
	     {
	       printf("\n%s not available in stock!\n", merch->name);
	     }
	 }
     }
}


///////////////////////////// REMOVE FROM CART //////////////////////////////////////


static int get_amount_item(item_t *item)
{
  int amount = 0;
  do
    {
      amount = ask_question_int("Amount: ");
    }
  while((amount < 1) || ( amount > item->amount));

  return amount;
}

void ioopm_remove_from_cart(ioopm_database_t *db)
{
  option_t result_cart = choose_cart(db);
  if (Successful(result_cart))
    {
      cart_t *cart = result_cart.value.ioopm_cart;

      database_print_cart(cart);
      char *wanted_item = ask_question_string("Name of item: ");
      option_t result_item = database_choose_item_in_cart(cart, wanted_item); 

      if(Successful(result_item))
	{
	  item_t *item = result_item.value.ioopm_item;
	  int amount = get_amount_item(item);

	  bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
	  if (!cont) return;
	  
	  database_remove_from_cart(cart, item, amount);
	}
      //free(wanted_item);
      release(wanted_item);
    }
}

///////////////////////////// CALCULATE COST //////////////////////////////////////


void ioopm_calculate_cost(ioopm_database_t *db)
{
  option_t result = choose_cart(db);
  if (Successful(result))
    {
      cart_t *cart = result.value.ioopm_cart;
      database_print_cart(cart);
    }
}

///////////////////////////// CHECKOUT //////////////////////////////////////

void ioopm_checkout(ioopm_database_t *db)
{
  option_t result_cart = choose_cart(db);
  
  if (Successful(result_cart))
    {
      cart_t *cart = result_cart.value.ioopm_cart;
      bool exists = database_items_in_cart_exist(db, cart); //Make sure the items in cart haven't been removed
      if(exists)
	{
	  database_print_cart(cart);
	  
	  bool cont = ioopm_ask_to_continue("Continue? [Y/N]");
	  if (!cont) return;
	  
	  database_checkout(db, cart);
	}
      else
	{
	  puts("An item in your cart no longer exists. Aborting checkout and deleting cart.");
	  database_delete_cart(db, cart);
	}
    }
}


///////////////////////////////// EVENT_LOOP /////////////////////////////////


void event_loop(ioopm_database_t *db)
{
  char *temp = NULL;
  char answer = ' ';
  while (answer != 'Q')
    {
      //free(temp);
      release(temp);
      temp = ask_question_menu("");
      answer = toupper(temp[0]);
      
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
	  ioopm_create_cart(db);
	}
      if (answer == 'M') //Remove cart
	{
	  ioopm_remove_cart(db);
	}
      if (answer == 'T') //Add to cart
	{
	  ioopm_add_to_cart(db);
	}
      if (answer == 'F') //Remove from cart
	{
	  ioopm_remove_from_cart(db);
	}
      if (answer == 'O') //Calculate cost
	{
	  ioopm_calculate_cost(db);
	} 
      if (answer == 'H') //Checkout
	{
	  ioopm_checkout(db);
	} 
    }
  //free(temp);
  release(temp);
  return;
}



///////////////////////////////MAIN/////////////////////////////////
int main()
{
  ioopm_database_t *db = database_create_database();
  event_loop(db);
  database_destroy_database(db);
  return 0;
}

