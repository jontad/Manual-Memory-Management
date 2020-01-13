#ifndef _BACKENDH_
#define _BACKENDH_

#include "common.h"
 

/**
 * @file backend.h
 * @author Emanuel Jansson & Jonathan Tadese
 * @date 1 Nov 2019
 * @brief Backend for public functions that run webstore
 */



/// @brief contains name, desc, price, available amount and location of merchandise 
//typedef struct merch merch_t;

/// @brief contains information to build hash table
//typedef struct hash_table hash_table_t;


/// @brief Creates a database with hash tables of merch, shelves and carts
/// @return a new empty database
database_t *database_create_database();
  
/// @brief Add merch into the database
/// @param db database to add merch to
/// @param name name of merchandise
/// @param desc description of merchandise
/// @param price price of merchandise
merch_t *database_add_merch(database_t *db, char *name, char *desc, unsigned int price);


/// @brief sorts a list
/// @param list list to be sorted
/// @return sorted list
char **database_sort_list(list_t *list);

/// @brief remove merch from database
/// @param db database to remove merch from
/// @param merch merch to remove
void database_remove_merch(database_t *db, merch_t *merch);

/// @brief edit name of merch
/// @param db database containing merchandise
/// @param new_name new name of merchandise
/// @param merch merchandise to be edited
/// @return merch with edited name
merch_t *database_edit_name(database_t *db, char *new_name, merch_t *merch);

/// @brief edit description of merch
/// @param merch merchandise to be edited
/// @param new_desc new description of merchandise
void database_edit_desc(merch_t *merch, char *new_desc);

/// @brief edit price of merch
/// @param merch merchandise to be edited
/// @param new_price new price of merchandise
void database_edit_price(merch_t *merch, int new_price);


/// @brief print shelf and amount of merch
/// @param merch merch to show stock of
void database_show_stock(merch_t *merch);

/// @brief creates a new empty shopping cart
/// @param db database of merch
/// @return pointer to cart 
cart_t *database_create_cart(database_t *db);

/// @brief removes a shopping cart from database and frees its memory
/// @param db database of merch
/// @param cart cart to remove
void database_remove_cart(database_t *db, cart_t *cart);

/// @brief adds an item to a specific cart
/// @param db database of merch
/// @param cart cart to add merch to
/// @param merch merch to add to cart
/// @param amount how many of the merch to add
void database_add_to_cart(database_t *db, cart_t *cart, merch_t *merch, int amount);

/// @brief choose a specific item in a cart
/// @param cart cart that contains items to be chosen
/// @param wanted_item name of item
/// @return Option_t containing item if successful 
option_t database_choose_item_in_cart(cart_t *cart, char *wanted_item);


/// @brief removes an item from a specific cart
/// @param cart cart to remove from
/// @param item item to remove
/// @param amount amount to remove
void database_remove_from_cart(cart_t *cart, item_t *item, int amount);

/// @brief remove a given cart and decrease the stock for merch in the cart
/// @param db database in cart
/// @param cart cart to checkout
void database_checkout(database_t *db, cart_t *cart);

/// @brief free the entire database
/// @param db database in cart
void database_destroy_database(database_t *db);

/// @brief prints the items in cart, their price, amount and the total cost of everything in the cart
/// @param cart the cart to print
void database_print_cart(cart_t *cart);

/// @brief increase stock of merch at a particular shelf
/// @param db database of merch
/// @param merch merchandise to be replenished
/// @param new_shelf shelf where merchandise will have stock
void database_replenish_stock(database_t *db, merch_t *merch, shelf_t *new_shelf);

/// @brief checks that every item in the given cart still exists in the database
/// @param db database containing cart and merch
/// @param cart cart to check
/// @return true if everything exists, false if something doesn't exist
bool database_items_in_cart_exist(database_t *db, cart_t *cart);

/// @brief choose merchandise from merch in database
/// @param db database to choose merch from
/// @param index index of merch to choose
/// @return chosen merch
option_t database_choose_merch(database_t *db, int index);

/// @brief free and remove a cart from a database
/// @param db database to remove cart from
/// @param cart cart to remove
void database_delete_cart(database_t *db, cart_t *cart);

/// @brief create a shelf
/// @param shelf_name name of shelf
/// @param amount amount of merch stored on the shelf
/// @return the created shelf
shelf_t *database_create_shelf(char *shelf_name, int amount);
#endif
