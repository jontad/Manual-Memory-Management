 #include <stdio.h>

#include "hash_table.h"
#include "inlupp_linked_list.h"

#include "backend.h"
#include "common.h"
#include "utils.h"

#include "../src/refmem.h"

///////////////////////////////// MISC ////////////////////////////////////////

bool ask_to_continue(char *question)
{
  char *answer = NULL;
  bool cont = false;
  bool quit = false;

  do //asks to continue listing merchandise after printing 20 items
    {
      free(answer);
      answer = ask_question_string(question);

      cont = (toupper(answer[0]) == 'Y' && (strlen(answer) == 1));
      quit = (toupper(answer[0]) == 'N' && (strlen(answer) == 1));
    }
  while (!cont && !quit);

  free(answer);
  return !quit;
}


static void print_item(char *name, char *desc, int price)
{
  printf("\nName: %s\n", name);
  printf("Description: %s\n", desc);
  printf("Price: %d\n\n", price);
}


/////////////////////////////////// ADD /////////////////////////////////////////////////


void add_merch(database_t *db)
{
  char *name = ask_question_string("Name of merchandise: ");
  option_t result = hash_table_lookup(db->merch_ht, str_elem(name)); //Check if a merch of that name already exists
  
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
      free(name);
    }
}

/////////////////////////////////// LIST ///////////////////////////////////////


void list_merch(database_t *db)
 {
   list_t *names = hash_table_keys(db->merch_ht); // generate list
   retain(names);
   size_t size = inlupp_linked_list_size(names);
   char **merch_list = database_sort_list(names);             // sort list

   if(size == 0)
     {
       puts("\nNo merchandise in database!");
     }
   
   for (int i = 0; i < size; ++i)
     {
       if (((i+1) % 3) == 0) 
	 {
	   bool cont = ask_to_continue("Continue listing? [Y/N]");
	   if (!cont) break;
	 }
       printf("%d. %s\n", i+1, merch_list[i]); //i+1: remember, arrays start a 0
     }
   printf("\n");
   release(merch_list);
   release(names);
   inlupp_linked_list_destroy(names);
 }


option_t choose_merch(database_t *db) //choose merch from list with number
{  
  list_merch(db);

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

void remove_merch(database_t *db)
{
  option_t result = choose_merch(db);

  if(Successful(result))
    {
      bool cont = ask_to_continue("Continue? [Y/N]");
      if (!cont) return;
      
      merch_t *merch = result.value.merch;
      retain(merch);
      database_remove_merch(db, merch);
    }
}


///////////////////////////////// EDIT ///////////////////////////////////////////

void event_loop_edit(database_t *db, merch_t *merch)
{
  char *answer = NULL;
  char *new_name = NULL;
  option_t lookup_result;
  do
    {
      free(answer);
      answer = ask_question_string("Edit [n]ame\nEdit [d]escription\nEdit [p]rice\n[Q]uit\n");
      if(strlen(answer) == 1)
	{
	  if(toupper(answer[0]) == 'N')
	    {
	      new_name = ask_question_string("New name of merchandise: ");
	      lookup_result = hash_table_lookup(db->merch_ht, str_elem(new_name));

	      bool cont = ask_to_continue("Continue? [Y/N]");
	      if (cont)
		{
		  if (Unsuccessful(lookup_result))
		    {
		      free(merch->name);
		      merch = database_edit_name(db, new_name, merch);
		      print_item(merch->name, merch->desc, merch->price_per_unit);

		    }
		  else
		    {
		      printf("%s already exists!\n", new_name);
		      free(new_name);
		    }
		}
	      else
		{
		  free(new_name);
		}
	    }
	  if(toupper(answer[0]) == 'D')
	    {
	      char *new_desc = ask_question_string("New description of merchandise: ");	    
	      bool cont = ask_to_continue("Continue? [Y/N]");
	      if (cont)
		{
		  free(merch->desc);
		  database_edit_desc(merch, new_desc);
		  print_item(merch->name, merch->desc, merch->price_per_unit);
		}
	      else
		{
		  free(new_desc);
		  print_item(merch->name, merch->desc, merch->price_per_unit);
		}
	    }
	  if(toupper(answer[0]) == 'P')
	    {
	      int new_price = ask_question_int("New price of merchandise: ");
	      bool cont = ask_to_continue("Continue? [Y/N]");
	      if (cont)
		{
		  database_edit_price(merch, new_price);
		}
	    }
	}
    }
  while(toupper(answer[0]) != 'Q' || strlen(answer) != 1);

  free(answer);       
}

void edit_merch(database_t *db)
{
  option_t result = choose_merch(db);
  if(Successful(result))
    {
      merch_t *merch = result.value.merch;
      retain(merch);
      print_item(merch->name, merch->desc, merch->price_per_unit);
      event_loop_edit(db, merch); 
    }
}

///////////////////////////// SHOW STOCK ////////////////////////////////////////////



void show_stock(database_t *db)
{
  option_t result = choose_merch(db);
  if(Successful(result))
    {
      merch_t *merch = result.value.merch;
      retain(merch);
      database_show_stock(merch);
    }
}




///////////////////////////// REPLENISH ////////////////////////////////////////////


void replenish_stock(database_t *db)
{
  option_t result = choose_merch(db);
  if(Successful(result))
    {
      merch_t *merch = result.value.merch;
      retain(merch);
      
      char *shelf_name = ask_question_shelf("Storage location: ");
      int amount = ask_question_int("Amount: ");

      shelf_t *new_shelf = database_create_shelf(shelf_name, amount);
      retain(new_shelf);
      
      database_replenish_stock(db, merch, new_shelf);
    }
}


/////////////////////////////// CREATE CART ///////////////////////////////


void create_cart(database_t *db)
{
  cart_t *cart = database_create_cart(db);
  printf("\nCart ID: %u\n", cart->id);
}


/////////////////////////////// REMOVE CART ////////////////////////////////


//Note: array_from_list_id and list_id are modified versions of list_merch
 static unsigned int *array_from_list_id(list_t *list) //returns a sorted list of id
{
  link_t *current_link = list->first;
  retain(current_link);
  
  size_t size = inlupp_linked_list_size(list);
  
  unsigned int *id_list = allocate_array(size, sizeof(unsigned int), NULL);
  
  for(int i = 0; i < size; ++i)
    {
      id_list[i] = current_link->value.u_int;
      release(current_link);
      current_link = current_link->next;
      retain(current_link);
    }
  return id_list;
}


static void list_id(database_t *db)
 {
   list_t *id = hash_table_keys(db->carts); // generate list
   retain(id);

   size_t size = inlupp_linked_list_size(id);
   unsigned int *id_list = array_from_list_id(id);
   retain(id_list);

   if(size == 0)
     {
       puts("\nNo carts in database!");
     }
   
   for (int i = 0; i < size; ++i)
     {
       if (((i+1) % 21) == 0)
	 {
	   bool cont = ask_to_continue("Continue listing? [Y/N]");
	   if (!cont) break;
	 }
       printf("Cart ID: %u\n", id_list[i]);
     }
   printf("\n");
   release(id);
   inlupp_linked_list_destroy(id);
   release(id_list);
 }



static option_t choose_cart(database_t *db)
{
  list_id(db);
    
  if(db->carts->size != 0)
    {
      int result = 0;
      option_t cart;
      
      do
	{
	  result = ask_question_int("Choose cart by cart ID: ");
	  cart = hash_table_lookup(db->carts, int_elem(result));
      
	}
      while((result < 0) || (result > db->id_counter) || Unsuccessful(cart));
      return cart;
    }
  
  return Failure();
}

void remove_cart(database_t *db)
{
  option_t result = choose_cart(db);
  if (Successful(result))
    {
      bool cont = ask_to_continue("Continue? [Y/N]");
      if (!cont) return;
      cart_t *cart = result.value.cart;
      retain(cart);
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

void add_to_cart(database_t *db)
{
  option_t result_carts = choose_cart(db); //Choose cart
  if (Successful(result_carts))
    {
      cart_t *cart = result_carts.value.cart;
      retain(cart);
      
      option_t result_merch = choose_merch(db); //Choose merch
      
      if(Successful(result_merch))
	{
	  merch_t *merch = result_merch.value.merch;
	  retain(merch);
	  
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
	  release(merch);
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

void remove_from_cart(database_t *db)
{
  option_t result_cart = choose_cart(db);
  if (Successful(result_cart))
    {
      cart_t *cart = result_cart.value.cart;
      retain(cart);

      database_print_cart(cart);
      char *wanted_item = ask_question_string("Name of item: ");
      option_t result_item = database_choose_item_in_cart(cart, wanted_item); 

      if(Successful(result_item))
	{
	  item_t *item = result_item.value.item;
	  retain(item);
	  int amount = get_amount_item(item);

	  bool cont = ask_to_continue("Continue? [Y/N]");
	  if (!cont) return;
	  
	  database_remove_from_cart(cart, item, amount);
	  release(item);
	}
      free(wanted_item);
    }
}

///////////////////////////// CALCULATE COST //////////////////////////////////////


void calculate_cost(database_t *db)
{
  option_t result = choose_cart(db);
  if (Successful(result))
    {
      cart_t *cart = result.value.cart;
      retain(cart);
      database_print_cart(cart);
      release(cart);
    }
}

///////////////////////////// CHECKOUT //////////////////////////////////////

void checkout(database_t *db)
{
  option_t result_cart = choose_cart(db);
  
  if (Successful(result_cart))
    {
      cart_t *cart = result_cart.value.cart;
      retain(cart);
      bool exists = database_items_in_cart_exist(db, cart); //Make sure the items in cart haven't been removed
      if(exists)
	{
	  database_print_cart(cart);
	  
	  bool cont = ask_to_continue("Continue? [Y/N]");
	  if (!cont) return;
	  
	  database_checkout(db, cart);
	}
      else
	{
	  puts("An item in your cart no longer exists. Aborting checkout and deleting cart.");
	  database_delete_cart(db, cart);
	}
      release(cart);
    }
}


///////////////////////////////// EVENT_LOOP /////////////////////////////////


void event_loop(database_t *db)
{
  char *temp = NULL;
  char answer = ' ';
  while (answer != 'Q')
    {
      free(temp);
      temp = ask_question_menu("");
      answer = toupper(temp[0]);
      
      if (answer == 'A') //Add merch
	{
	  add_merch(db); 
	}
      if (answer == 'L') //List merch
	{
	  list_merch(db);
	}
      if (answer == 'R') //Remove merch
	{
	  remove_merch(db);
	}
      if (answer == 'E') //Edit merch
	{
	  edit_merch(db);
	}
      if (answer == 'S') //Show stock
	{
	  show_stock(db);
	}
      if (answer == 'P')//Replenish stock
	{
	  replenish_stock(db);
	}
      if (answer == 'C') //Create cart
	{
	  create_cart(db);
	}
      if (answer == 'M') //Remove cart
	{
	  remove_cart(db);
	}
      if (answer == 'T') //Add to cart
	{
	  add_to_cart(db);
	}
      if (answer == 'F') //Remove from cart
	{
	  remove_from_cart(db);
	}
      if (answer == 'O') //Calculate cost
	{
	  calculate_cost(db);
	} 
      if (answer == 'H') //Checkout
	{
	  checkout(db);
	} 
    }
  free(temp);
  return;
}



///////////////////////////////MAIN/////////////////////////////////
int main()
{
  database_t *db = database_create_database();
  retain(db);
  event_loop(db);

  release(db);
  database_destroy_database(db);
  //shutdown();
  return 0;
}

