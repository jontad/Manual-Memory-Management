#ifndef _FRONTENDH_
#define _FRONTENDH_

#include "common.h"


/**
 * @file frontend.h
 * @author Emanuel Jansson & Jonathan Tadese
 * @date 1 Nov 2019
 * @brief Frontend for public functions that run webstore
 */


/// @brief contains name, desc, price, available amount and location of merchandise 
//typedef struct merch merch_t;

/// @brief contains information to build hash table
///typedef struct hash_table hash_table_t;

/// @brief Creates a database with hash tables of merch, shelves and carts
/// @return a new empty database
database_t *create_database();
  
/// @brief Add merch into the database
/// @param db database to add merch to
void add_merch(database_t *db);

/// @brief print entries to the terminal 20 at a time 
/// @param db database containing merch to list
void list_merch(database_t *db);

/// @brief remove merch from database
/// @param db database to remove merch from
void remove_merch(database_t *db);

/// @brief edit name, description or price of merch
/// @param db database containing merch to edit
void edit_merch(database_t *db);

/// @brief print shelf and amount of merch
/// @param db database containing merch
void show_stock(database_t *db);

/// @brief increase stock of merch at a particular shelf
/// @param db database of merch
void replenish_stock(database_t *db);

/// @brief removes a shopping cart from database and frees its memory
/// @param db database of merch
void remove_cart(database_t *db);

/// @brief adds an item to a specific cart
/// @param db database of merch
void add_to_cart(database_t *db);

/// @brief removes an item from a specific cart
/// @param db database of merch
void remove_from_cart(database_t *db);

/// @brief calculates cost of items in a specific cart
/// @param db database in cart
void calculate_cost(database_t *db);

/// @brief remove a specific cart and decrease the stock for merch in the cart
/// @param db database in cart
void checkout(database_t *db);

/// @brief get input from the user whether to continue or not
/// @return true if input is 'y', false if 'n'
bool ask_to_continue();

#endif
