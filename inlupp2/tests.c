#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>

#include "hash_table.h"
#include "inlupp_linked_list.h"
#include "common.h"
#include "backend.h"
#include "utils.h"
//#include "../src/linked_list.h"
#include "../src/refmem.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}


bool ask_to_continue()
{
  char *answer = NULL;
  bool cont = false;
  bool quit = false;

  do //asks to continue listing merchandise after printing 20 items
    {
      free(answer);
      answer = ask_question_string("[C]ontinue? [Y/N]: ");

      cont = (toupper(answer[0]) == 'Y' && (strlen(answer) == 1));
      quit = (toupper(answer[0]) == 'N' && (strlen(answer) == 1));
    }
  while (!cont && !quit);

  free(answer);
  return !quit;
}


static item_t *item_create_for_testing(merch_t *merch, int amount)
{
  item_t *item = calloc(1, sizeof(item_t));
  item->name = merch->name;
  item->amount = amount;
  item->price_per_unit = merch->price_per_unit;
  item->pointer = merch;
  return item;
} 



//The tests use a modified version of the database, since it doesn't have to free ask_question
/////////////////////////////////////////////////////////////////////////////////////////////////
static void free_items_in_cart(link_t **element, void *extra)
{
  release((*element)->value.item);
}

static void free_carts_apply_func(elem_t key, elem_t *value, void *extra)
{
  inlupp_linked_apply_to_all(value->cart->basket, free_items_in_cart, NULL);
  inlupp_linked_list_destroy(value->cart->basket);
  release(value->cart);
}

static void clear_stock(list_t *stock)
{
  elem_t element;
  for (int i = 0; i < stock->list_size; ++i)
    {
      element = inlupp_linked_list_get(stock, i);
      //free(element.shelf->shelf_name); This is not needed when not taking input from ask_question
      release(element.shelf);
    }
}


static void destroy_merch(database_t *db, merch_t *merch) //Note: remember to remove from hash table as well!
{
  list_t *stock = merch->stock;
  link_t *current_location = stock->first;
  
  while (current_location != NULL)  //1. remove from shelves_ht hash table
    {
      retain(current_location);
      elem_t shelf = str_elem(current_location->value.shelf->shelf_name);
      hash_table_remove(db->shelves_ht, shelf);
      current_location = current_location->next;
    }
  clear_stock(stock);
  inlupp_linked_list_destroy(stock); //2. free list of shelves_ht
  //free(merch->name); //These are not needed when not taking input from ask question!
  //free(merch->desc);
  release(merch);
}

static void free_merch_apply_func(elem_t key, elem_t *value, void *db)
{  
  //destroy_merch(db, value->merch);
  release(value->merch);
}

static void free_shelf_apply_func(elem_t key, elem_t *value, void *db)
{  
  release(value->shelf);
}

void tests_destroy_database(database_t *db)
{
  //hash_table_apply_to_all(db->carts, free_carts_apply_func, NULL);
  hash_table_destroy(db->carts);
  //hash_table_apply_to_all(db->merch_ht, free_merch_apply_func, db);
  hash_table_destroy(db->merch_ht);
  //hash_table_apply_to_all(db->shelves_ht, free_shelf_apply_func, db);
  hash_table_destroy(db->shelves_ht);

  release(db);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void test_add_merch(void)
{
  database_t *db = database_create_database();
  database_add_merch(db, "1", "beskrivning", 10); //Add first merch
  option_t result = hash_table_lookup(db->merch_ht, str_elem("1"));
  CU_ASSERT_TRUE(result.success);
  
  char *desc = result.value.merch->desc;
  char *desc2 = "beskrivning";
  bool comparison = strcmp(desc, desc2) == 0;
  CU_ASSERT_TRUE(comparison);

  int price = result.value.merch->price_per_unit;
  CU_ASSERT_EQUAL(price, 10);
  
  int size = result.value.merch->stock->list_size;
  CU_ASSERT_EQUAL(size, 0);

  
  database_add_merch(db, "18", "beskrivning", 10); //Add first merch

  tests_destroy_database(db);

}

void test_remove_merch(void)
{
  database_t *db = database_create_database();

  merch_t *merch = database_add_merch(db, "namn", "beskrivning", 10); //Add a merch
  database_remove_merch(db, merch); //and remove it
  option_t result = hash_table_lookup(db->merch_ht, str_elem("namn"));
  CU_ASSERT_FALSE(result.success);
  
  tests_destroy_database(db);

}

void test_edit(void) 
{
  database_t *db = database_create_database();
  merch_t *merch  = database_add_merch(db, "namn", "beskrivning", 10); //Add a merch

  merch = database_edit_name(db, "nytt_namn", merch);
  database_edit_desc(merch, "ny_beskrivning");
  database_edit_price(merch, 20);   

  
  option_t result = hash_table_lookup(db->merch_ht, str_elem("namn"));
  CU_ASSERT_FALSE(result.success);

  result = hash_table_lookup(db->merch_ht, str_elem("nytt_namn"));
  CU_ASSERT_TRUE(result.success);

  char *desc = result.value.merch->desc;
  CU_ASSERT_TRUE(strcmp(desc, "ny_beskrivning") == 0);

  int price = result.value.merch->price_per_unit; 
  CU_ASSERT_EQUAL(price, 20);
  
  int size = result.value.merch->stock->list_size;
  CU_ASSERT_EQUAL(size, 0);
  
  tests_destroy_database(db);

}

void test_replenish(void)
{
  database_t *db = database_create_database();
  
  merch_t *merch_namn = database_add_merch(db, "namn", "beskrivning", 10); //Add first merch
  shelf_t *shelf_h20 = database_create_shelf("H20", 5);

  /*inlupp_linked_list_insert(merch_namn->stock, 0, shelf_elem(shelf_h20));
  printf("ref count h20: %d\n", rc(merch_namn->stock->first));
  
  shelf_t *shelf_b30 = database_create_shelf("B30", 10);
  inlupp_linked_list_insert(merch_namn->stock, 0, shelf_elem(shelf_b30));
  printf("ref count b30: %d\n", rc(merch_namn->stock->first));
  printf("ref count h20: %d\n", rc(merch_namn->stock->first->next));
  */
  
  database_replenish_stock(db, merch_namn, shelf_h20); //Replenish first merch
  
  option_t result = hash_table_lookup(db->merch_ht, str_elem("namn"));
  CU_ASSERT_TRUE(result.success);
  
  database_replenish_stock(db, merch_namn, shelf_h20); //Replenish first merch again
  
  int size = result.value.merch->stock->list_size;
  CU_ASSERT_EQUAL(size, 1);

  shelf_t *shelf = result.value.merch->stock->first->value.shelf;
  CU_ASSERT_TRUE(strcmp(shelf->shelf_name, "H20") == 0);
  CU_ASSERT_EQUAL(shelf->amount, 10);  
  
  shelf_t *shelf_b30 = database_create_shelf("B30", 10);
  database_replenish_stock(db, merch_namn, shelf_b30); //Replenish again
  
  
  shelf_t *shelf_a10 = database_create_shelf("A10", 10);
  database_replenish_stock(db, merch_namn, shelf_a10); //Replenish again
  
  result = hash_table_lookup(db->merch_ht, str_elem("namn"));
  CU_ASSERT_TRUE(result.success);

  
  size = result.value.merch->stock->list_size;
  CU_ASSERT_EQUAL(size, 3);
  char *shelf1 = result.value.merch->stock->first->value.shelf->shelf_name;
  char *shelf2 = result.value.merch->stock->first->next->value.shelf->shelf_name;
  char *shelf3 = result.value.merch->stock->last->value.shelf->shelf_name;
  
  CU_ASSERT_TRUE(strcmp(shelf1, "A10") == 0);
  CU_ASSERT_TRUE(strcmp(shelf2, "B30") == 0);

  CU_ASSERT_TRUE(strcmp(shelf3, "H20") == 0);

  //check that shelves_ht is updated properly
  result = hash_table_lookup(db->shelves_ht, str_elem("A10"));
  CU_ASSERT_TRUE(result.success);
  
  tests_destroy_database(db);

}

void test_create_cart(void)
{
  database_t *db = database_create_database();
  database_create_cart(db);

  option_t result = hash_table_lookup(db->carts, unsigned_elem(0));
  CU_ASSERT_TRUE(result.success);

  database_create_cart(db);
  result = hash_table_lookup(db->carts, unsigned_elem(1));
  CU_ASSERT_TRUE(result.success);

  result = hash_table_lookup(db->carts, unsigned_elem(0)); //Check that the original cart hasn't been overwritten
  CU_ASSERT_TRUE(result.success);
  
  tests_destroy_database(db);

}

void test_remove_cart(void)
{
  database_t *db = database_create_database();
  database_create_cart(db); //Cart 0
  database_create_cart(db); //Cart 1
  database_create_cart(db); //Cart 2
    
  cart_t *cart1 = hash_table_lookup(db->carts, unsigned_elem(1)).value.cart;
  database_remove_cart(db, cart1); //Remove cart 1
  
  option_t result = hash_table_lookup(db->carts, unsigned_elem(1));
  CU_ASSERT_FALSE(result.success);

  result = hash_table_lookup(db->carts, unsigned_elem(0));
  CU_ASSERT_TRUE(result.success);

  result = hash_table_lookup(db->carts, unsigned_elem(2));
  CU_ASSERT_TRUE(result.success);
  
  tests_destroy_database(db);
}

void test_add_to_cart(void)
{
  database_t *db = database_create_database();
  cart_t *cart = database_create_cart(db);
  merch_t *merch = database_add_merch(db, "namn", "beskrivning", 10); //Add merch
  shelf_t *shelf_h20 = database_create_shelf("H20", 10);
  database_replenish_stock(db, merch, shelf_h20); //Replenish merch

  database_add_to_cart(db, cart, merch, 10); //Add it to cart

  option_t result = hash_table_lookup(db->carts, unsigned_elem(0));

  item_t *item = result.value.cart->basket->first->value.item;
  
  CU_ASSERT_EQUAL(result.value.cart->basket->list_size, 1);
  CU_ASSERT_TRUE(strcmp(item->name, "namn") == 0);
  CU_ASSERT_EQUAL(item->amount, 10);
  CU_ASSERT_EQUAL(item->price_per_unit, 10);
  
  tests_destroy_database(db);
}

void test_remove_from_cart(void)
{
  database_t *db = database_create_database();
  cart_t *cart = database_create_cart(db); 

  merch_t *merch = database_add_merch(db, "namn", "beskrivning", 10); //Add merch
  shelf_t *shelf_h20 = database_create_shelf("H20", 50);
  database_replenish_stock(db, merch, shelf_h20); //Replenish merch

  database_add_to_cart(db, cart, merch, 25); //Add merch to cart
    
  
  item_t *item = cart->basket->first->value.item;
  database_remove_from_cart(cart, item, 5); //Tests only removing some

  
  option_t result = hash_table_lookup(db->carts, unsigned_elem(0));
  CU_ASSERT_TRUE(result.success);
  CU_ASSERT_EQUAL(result.value.cart->basket->first->value.item->amount, 20);
  
  char *name = result.value.cart->basket->first->value.item->name;
  option_t lookup_merch = hash_table_lookup(db->merch_ht, str_elem(name));
  CU_ASSERT_EQUAL(lookup_merch.value.merch->available_amount, 30);
    
  
  database_remove_from_cart(cart, item, 20); //Tests removing everything
  
  result = hash_table_lookup(db->carts, unsigned_elem(0));
  CU_ASSERT_TRUE(result.success);
  CU_ASSERT_EQUAL(result.value.cart->basket->list_size, 0);
  CU_ASSERT_EQUAL(result.value.cart->basket->first, NULL);
  CU_ASSERT_EQUAL(result.value.cart->basket->last, NULL);

  
  merch_t *merch_not_in_cart = database_add_merch(db, "hejsan", "not_in_cart", 10); //Add merch
  item_t *item_not_in_cart = item_create_for_testing(merch_not_in_cart, 10);
  
  database_remove_from_cart(cart, item_not_in_cart, 5); //Tests that removing item not in cart does nothing
  free(item_not_in_cart);
  tests_destroy_database(db);
}

void test_checkout(void)
{
  database_t *db = database_create_database();
  cart_t *cart = database_create_cart(db);
  merch_t *merch = database_add_merch(db, "namn", "beskrivning", 10); //Add merch
  shelf_t *shelf_h20 = database_create_shelf("H20", 50);
  database_replenish_stock(db, merch, shelf_h20); //Replenish it
  database_add_to_cart(db, cart, merch, 10); //Add it to cart

  database_checkout(db, cart); //Confirm checkout
  
  option_t result = hash_table_lookup(db->carts, unsigned_elem(0));
  CU_ASSERT_FALSE(result.success);
  
  tests_destroy_database(db);
}

void test_sort_list(void)
{
  list_t *list = inlupp_linked_list_create(equality_function_str);

  inlupp_linked_list_insert(list, 0, str_elem("c"));
  inlupp_linked_list_insert(list, 0, str_elem("a"));
  inlupp_linked_list_insert(list, 0, str_elem("b"));

  char **sorted_list = database_sort_list(list);

  char *a = sorted_list[0];
  char *b = sorted_list[1];
  char *c = sorted_list[2];
  
  CU_ASSERT_TRUE(strcmp("a", a) == 0);
  CU_ASSERT_TRUE(strcmp("b", b) == 0);
  CU_ASSERT_TRUE(strcmp("c", c) == 0);

  release(sorted_list);
  inlupp_linked_list_destroy(list);
}


void test_choose_item_in_cart(void)
{
  database_t *db = database_create_database();
  cart_t *cart = database_create_cart(db);
  merch_t *merch = database_add_merch(db, "namn", "beskrivning", 10); //Add merch
  shelf_t *shelf_h20 = database_create_shelf("H20", 50);
  database_replenish_stock(db, merch, shelf_h20); //Replenish it
  database_add_to_cart(db, cart, merch, 10); //Add it to cart
  
  option_t not_in_cart = database_choose_item_in_cart(cart, "fel_namn");
  CU_ASSERT_FALSE(not_in_cart.success);
  
  option_t in_cart = database_choose_item_in_cart(cart, "namn");
  CU_ASSERT_TRUE(Successful(in_cart));
  
  char *result = in_cart.value.item->name;
  CU_ASSERT_TRUE(strcmp("namn", result) == 0);

  tests_destroy_database(db);
}

void test_items_in_cart_exist(void)
{
  database_t *db = database_create_database();
  cart_t *cart = database_create_cart(db);
  merch_t *merch = database_add_merch(db, "namn", "beskrivning", 10); //Add merch
  shelf_t *shelf_h20 = database_create_shelf("H20", 50);
  database_replenish_stock(db, merch, shelf_h20); //Replenish it
  database_add_to_cart(db, cart, merch, 10); //Add it to cart

  bool exists = database_items_in_cart_exist(db, cart);
  CU_ASSERT_TRUE(exists);

  tests_destroy_database(db);
}

void test_choose_merch(void)
{
  database_t *db = database_create_database();
  database_add_merch(db, "namn", "beskrivning", 10); //Add merch

  option_t result = database_choose_merch(db, 1);
  CU_ASSERT_TRUE(result.success);
  /*
  char *name = result.value.merch->name;
  CU_ASSERT_TRUE(strcmp(name, "namn") == 0);
*/
  tests_destroy_database(db);
}

void test_resize(void)
{
  database_t *db = database_create_database();
  merch_t *merch1 = database_add_merch(db, "namn1", "beskrivning", 10); //Adds 23 merch
  database_add_merch(db, "namn2", "beskrivning", 10);
  database_add_merch(db, "namn3", "beskrivning", 10);
  database_add_merch(db, "namn4", "beskrivning", 10);
  database_add_merch(db, "namn5", "beskrivning", 10);
  database_add_merch(db, "namn6", "beskrivning", 10);
  database_add_merch(db, "namn7", "beskrivning", 10);
  merch_t *merch8 = database_add_merch(db, "namn8", "beskrivning", 10);
  merch_t *merch9 = database_add_merch(db, "namn9", "beskrivning", 10);
  database_add_merch(db, "namn10", "beskrivning", 10);
  database_add_merch(db, "namn11", "beskrivning", 10);
  database_add_merch(db, "namn12", "beskrivning", 10);
  database_add_merch(db, "namn13", "beskrivning", 10);
  database_add_merch(db, "namn14", "beskrivning", 10);
  database_add_merch(db, "namn15", "beskrivning", 10);
  database_add_merch(db, "namn16", "beskrivning", 10);
  database_add_merch(db, "namn17", "beskrivning", 10);
  database_add_merch(db, "namn18", "beskrivning", 10);
  database_add_merch(db, "namn19", "beskrivning", 10);
  database_add_merch(db, "namn20", "beskrivning", 10);
  database_add_merch(db, "namn21", "beskrivning", 10); 
  database_add_merch(db, "namn22", "beskrivning", 10);
  database_add_merch(db, "namn23", "beskrivning", 10);

  printf("name: %s \n",merch9->name);
  database_remove_merch(db, merch9); //test removing the 23rd merch after continue listing
  database_remove_merch(db, merch8); //test removing the 22rd merch before continue listing
  database_remove_merch(db, merch1); //test removing the first merch after continue listing
  
  option_t namn9 = hash_table_lookup(db->merch_ht, str_elem("namn9"));
  CU_ASSERT_FALSE(namn9.success);

  option_t namn8 = hash_table_lookup(db->merch_ht, str_elem("namn8"));
  CU_ASSERT_FALSE(namn8.success);

  option_t namn1 = hash_table_lookup(db->merch_ht, str_elem("namn1"));
  CU_ASSERT_FALSE(namn1.success);
  
  
  tests_destroy_database(db);
}



int main()
{
  CU_pSuite test_suite1 = NULL;
  
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();


  test_suite1 = CU_add_suite("Test Suite 1: webstore backend", NULL, NULL);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
  
  CU_add_test(test_suite1, "test add merch", test_add_merch);
  CU_add_test(test_suite1, "test remove merch", test_remove_merch);
  CU_add_test(test_suite1, "test edit merch", test_edit);
  CU_add_test(test_suite1, "test replenish", test_replenish);
  CU_add_test(test_suite1, "test create cart", test_create_cart);
  CU_add_test(test_suite1, "test remove cart", test_remove_cart);
  CU_add_test(test_suite1, "test add to cart", test_add_to_cart);
  CU_add_test(test_suite1, "test remove from cart", test_remove_from_cart);
  CU_add_test(test_suite1, "test checkout", test_checkout);
  /* CU_add_test(test_suite1, "test resize", test_resize);*/
  CU_add_test(test_suite1, "test sort list", test_sort_list);
  CU_add_test(test_suite1, "test chosen item in cart", test_choose_item_in_cart);
  CU_add_test(test_suite1, "test items in cart exist", test_items_in_cart_exist);
  CU_add_test(test_suite1, "test choose merch", test_choose_merch);

  
  CU_basic_set_mode(CU_BRM_VERBOSE);
  if(!CU_basic_run_tests()) // Shutdown only when passing all tests
    {
      shutdown();
    }
  CU_cleanup_registry();
  return CU_get_error();
}
