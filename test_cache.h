/* 
 * TEST_CACHE.H
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 * Each test func must return an int indicating success (1); failure (0); or
 * a terminating fault (-1), where an error message is printed.
 * 
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "sim_cache.h"
#include "file_sys.h"

/*** TESTING FRAMEWORK **/

// given an array of function pointers, run each test function and output
// its results (or errors)  
void run_tests();


/*** CACHE UTIL TESTS ***/

int test_create_free_cache();

int test_push_back_cache();

int test_pop_front_cache();

int test_find_in_cache();

int test_free_cache_item();

/*** FILE UTIL TESTS ***/

int test_read_file_to_buf();

int test_write_buf_to_file();

/*** SIM CACHE TESTS ***/

int test_get_substr();

int test_extract_command_goods();

int test_extract_command_bads();

int test_find_in_cache();