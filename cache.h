/* 
 * CACHE.H
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "file_sys.h"

typedef struct cache_t* C_T;


typedef struct cache_file_t {
    unsigned char *data; // malloc'd buffer containing len bytes of data
    char *name; // name of file in current directory
    int len; // length of file, in bytes

    int max_age; // expiration time of file, in seconds
    clock_t expiration; // time at which expiration will occur (ticks)
    clock_t last_retrieved; // time of last GET call on file
} cache_file_t; 

// macro for an empty 'null' value of the cache_file_t type.
#define NULL_FILE (cache_file_t){NULL, NULL, 0, 0, 0, 0};

/*** CACHE FILE UTIL FUNCS ***/

// returns file if it exists in the cache; otherwise returns NULL_FILE
cache_file_t retrieve_file_struct(C_T cache, char *file_name);

// prints out information for the given file
void print_file_struct(cache_file_t file);


/**** CACHE UTIL FUNCS ****/

// evicts one item from the cache, according to eviction policies
void *evict_one(C_T cache);

// initializes a new cache with a given capacity (unsigned int)
void *create_cache(int cap);

// frees memory associated with a given cache
void free_cache(C_T cache);

// returns number of items currently in cache
int size_of_cache(C_T cache);

// returns capacity of cache
int cap_of_cache(C_T cache);

// adds a new file to end / back of the cache (newest)
void *push_back_cache(C_T cache, char *file_name, int max_age);

// removes a given file in the cache
void *remove_file_cache(C_T cache, char *file_name);

// updates item in cache with updated time information
void *update_item_cache(C_T cache, char *file_name, cache_file_t new_item);

// prints out the contents of the cache
void print_cache(C_T cache);
