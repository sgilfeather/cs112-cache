/* 
 * CACHE.C
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include "cache.h"

/*** CACHE FILE STRUCT PTR: defined in header ***/

/*** CACHE LIST-ITEM STRUCT PTR ***/
typedef struct cache_item_t {
    cache_file_t file;
    struct cache_item_t *next; // pointer to next cache_item_t 
} *cache_item_t;


/*** CACHE STRUCT ***/
struct cache_t {
    cache_item_t head; // linked list representing cache items
    cache_item_t tail; // last item in linked list
    int cap; // number of filled spots (items) in cache
    int size;
};
// as defined in header, (struct cache_t *) is type-def'd to C_T


/*** STATIC HELPER FUNC DECLARATIONS ***/

// traverses to, and returns, file; returns NULL if not found
static int find_in_cache(C_T cache, char *file_name, 
                                 cache_item_t *item_add);

// creates a new cache_item_t pointer with memory for the file's buffer
static cache_item_t new_cache_item(char *file_name, int max_age);

// given a malloc'd cache_item_t, frees its associated memory
static void free_cache_item(cache_item_t item);

// removes item at index "index" in the cache's linked list
static void *remove_at_cache(C_T cache, int index);


/*
 * @note    eviction policy: if all files have been accessed before, evict the
 *          least-recently accessed file; otherwise, evict the oldest file
 */
void *evict_one(C_T cache)
{
    cache_item_t curr = cache->head;
    int num_not_retrieved = 0;
    char *few_retrieved = NULL;
    char *many_retrieved = NULL;

    clock_t now = clock();
    clock_t least_recent = now;

    while (curr != NULL)
    {
        char *name = (curr->file).name;
        // printf("currently, name is: %s\n", name);

        if ((curr->file).expiration <= now) {
            remove_file_cache(cache, name);
            delete_file(name);
            return (void *)cache;
        }

        if ((curr->file).last_retrieved == 0) {
            num_not_retrieved++;
            printf("%s has never been retrieved\n", name);
            if (few_retrieved == NULL)
                few_retrieved = name; // record oldest never-retrieved
        }
        
        if ( (curr->file).last_retrieved != 0
                && (curr->file).last_retrieved < least_recent) {
            least_recent = (curr->file).last_retrieved;
            many_retrieved = name; // update least-recently retrieved
            printf("we found a file that has been retrieved older: \n");

        }
        curr = curr->next;
    }
    
    if (num_not_retrieved < 2) { // if 0 or 1 have never been retreived...
        printf("deleting if many retrieved\n");
        // printf("retrieved name: %s", retrieved);
        delete_file(many_retrieved);
        remove_file_cache(cache, many_retrieved);
    }
    else {
        printf("deleting if few retrieved\n");
        delete_file(few_retrieved);
        remove_file_cache(cache, few_retrieved);
    }

    return (void *)cache;
}



/* create_cache()
 * @brief:  initializes a new cache
 * @param   cap: maximum capacity of cache (unsigned int)
 * @returns a struct cache_t pointer
 * @note    cache must be able to hold at least one item
 */ 
void *create_cache(int cap)
{
    if (cap == 0) {
        return NULL;
    }

    C_T new_cache = (C_T)malloc(sizeof(struct cache_t));
    new_cache->head = NULL;
    new_cache->tail = NULL;
    new_cache->cap = cap;
    new_cache->size = 0;

    return (void *)new_cache;
}


/* free_cache()
 * @brief: initializes a new cache
 * @param  cache: a struct cache_t pointer
 * @returns: none, but frees memory associated with cache pointer
 */ 
void free_cache(C_T cache)
{  
    if (cache == NULL)
        return;  

    cache_item_t curr = cache->head;

    // free the linked list of cache items
    while (curr != NULL) {
        cache_item_t to_free = curr;
        curr = curr->next;
        free_cache_item(to_free); // frees to_free as well
    }

    free(cache);
    return;
}


/* size_of_cache()
 * @brief   returns number of 'items' currently stored in cache
 * @param   cache: a struct cache_t pointer
 * @returns number of items, an int
 * @note    if cache is invalid, returns -1
 */ 
int size_of_cache(C_T cache)
{
    if (cache == NULL)
        return -1; 

    return cache->size;
}

/* cap_of_cache()
 * @brief   returns capacity, maximum items that the cache can store
 * @param   cache: a struct cache_t pointer
 * @returns capacity, an int
 * @note    if cache is invalid, returns -1
 */ 
int cap_of_cache(C_T cache)
{
    if (cache == NULL)
        return -1; 

    return cache->cap;
}



/* push_back_cache()
 * @brief   adds a new file to the back of the cache  
 * @param   cache: a struct cache_t pointer
 * @param   file: name of file to add to back of cache
 * @returns modified struct cache_t pointer, cast to void pointer
 * @note    files that haven't been retreived are init'd with a last_retr.
 *          value of exactly 0.
 */  
void *push_back_cache(C_T cache, char *file_name, int max_age)
{
    if (cache == NULL)
        return NULL; 

    cache->size = cache->size + 1; // update size of cache

    cache_item_t new_item = new_cache_item(file_name, max_age);

    if (cache->head == NULL && cache->tail == NULL) {
        cache->head = new_item;
        cache->tail = new_item;
        return (void *)cache;   
    }
    
    // otherwise, append new cache_file_t to back of cache list
    (cache->tail)->next = new_item;
    cache->tail = new_item; // back of list points to new item 

    return (void *)cache;
}


/* remove_at_cache() 
 * @brief   removes a file from the given index in the cache
 * @param   cache: a struct cache_t pointer
 * @param   index: index of file to remove, in cache's linked list
 * @returns modified struct cache_t pointer, cast to void pointer
 * @note    if cache is empty, return cache unchanged
 */
static void *remove_at_cache(C_T cache, int index)
{
    if (cache == NULL)
        return NULL; 

    else if (cache->head == NULL || index < 0 || index >= cache->size)
        return (void *)cache; // if cache list is empty, or bad index

    cache_item_t curr = cache->head;

    if (cache->tail == cache->head) { // if list has one item, repoint tail
        cache->head = NULL;
        cache->tail = NULL;
        free_cache_item(curr);
    }
    else if (index == 0) { // if item is at front of list, you're there!
        cache->head = (cache->head)->next;
        free_cache_item(curr);
    }
    else { // traverse to node BEFORE target (at end, i = index - 1)
        int i;
        for (i = 0; i < index - 1; i++)
            curr = curr->next; 

        cache_item_t target = curr->next;
        if (target->next == NULL) { // if we're at the end of the list
            cache->tail = NULL;
        }
        else {
            curr->next = target->next;
        }
        free_cache_item(target);
    }

    cache->size = cache->size - 1; // update num of items in cache
    return (void *)cache;
}


/* remove_at_cache() 
 * @brief   removes a given file from the cache
 * @param   cache: a struct cache_t pointer
 * @param   file_name: name of file to remove
 * @returns modified struct cache_t pointer, cast to void pointer
 * @note    if cache is empty, return cache unchanged
 */
void *remove_file_cache(C_T cache, char *file_name)
{
    int ind = find_in_cache(cache, file_name, NULL);
    
    if (ind != -1) // if file exists in the cache
        cache = (C_T)remove_at_cache(cache, ind);

    return (void *)cache;
}


/* get_file_struct()
 * @brief   
 * @param   cache: a struct cache_t pointer
 * @param   file_name: name of file to search for
 * @returns modified struct cache_t pointer, cast to void pointer
 * @note    if cache is empty, return cache unchanged
 */ 
cache_file_t retrieve_file_struct(C_T cache, char *file_name)
{
    cache_item_t item = NULL;
    int ind = find_in_cache(cache, file_name, &item);

    if (ind == -1) {
        return NULL_FILE; // macro for 'null' of cache_file_t type
    }

    return item->file;
} 


/* print_file_struct()
 * @brief   prints information about the given file
 * @param   file: name of file to print
 * @returns none, but prints info to std::stderr
 */ 
void print_file_struct(cache_file_t file)
{
    if (file.name == NULL && file.data == NULL) {
        fprintf(stderr, "NULL_FILE.\n");
        return;
    }

    // milliseconds since last retrieval
    double ret = (double)clock() - (double)file.last_retrieved;
    ret = 1000 * (ret / CLOCKS_PER_SEC);

    printf("FILE %s: length %u, max_age %i, ", 
           file.name, file.len, file.max_age);
    
    if (file.last_retrieved == 0) 
        printf("never retrieved.\n");
    else
        printf("retrieved %0.2lf ms ago.\n", ret);
}


/* update_item_cache()
 * @brief   updates a given item in the cache with new cache info (after
 *          a retrieval).
 * @param   cache
 * 
 */ 
void *update_item_cache(C_T cache, char *file_name, cache_file_t our_file)
{
    cache_item_t to_update = NULL;
    clock_t now = clock();
    find_in_cache(cache, file_name, &to_update);

    if (to_update != NULL) {
        our_file.last_retrieved = now;
        our_file.expiration = now + (our_file.max_age * CLOCKS_PER_SEC);

        to_update->file = our_file;
    }
    return (void *)cache;
}


/* print_cache()
 * @brief   prints out the contents of the cache
 * @param   cache   cache instance to print
 * @returns none
 */
void print_cache(C_T cache)
{
    if (cache == NULL)
        return;

    cache_item_t curr = cache->head;
    int ind = 1;

    while (curr != NULL) {
        printf("\t%i: ", ind);
        print_file_struct(curr->file);
        ind++;

        curr = curr->next;
    }

    int i; 
    for (i = ind; i <= cache->cap; i++)
        printf("\t%i: NO FILE\n", i);
}


/*** STATIC HELPER FUNCTIONS ***/


/* find_in_cache()
 * @brief   traverses to and returns the cache_item_t pointer in the cache
 *          list with the given file_name.
 * @param   file_name: name of file;
 * @param   cache: a struct cache_t pointer
 * @param   item_add: pointer to store retrieved item in.
 * @returns index of the retrieved item
 * @note    if file is not found, cache_item is set to NULL, and
 *          -1 is returned. 
 * 
 * @note    caller can pass item_add as NULL, if just retrieving int index
 */ 
static int find_in_cache(C_T cache, char *file_name, 
                                  cache_item_t *item_add)
{
    int ind = 0;
    if (cache == NULL)
        return -1; 

    cache_item_t curr = cache->head; // struct cache_item_t * --> cache_item_t
    while (curr != NULL) {
        if (strncmp((curr->file).name, 
        file_name, 
        strlen(file_name)) == 0){
            // if caller is using item_add; otherwise, don't update
            
            if (item_add != NULL) {
                *item_add = curr;
            }

            return ind;
        }

        curr = curr->next;
        ind++;
    }
    // file with file_name was not found.
    if (item_add != NULL)
        *item_add = NULL;

    return -1; 
}


/* new_cache_item()
 * @brief   creates a new cache_item_t pointer that stores a new
 *          cache_file_t struct (with a malloc'd buffer for the file data)
 * @param   file_name   name of file to store in item's cache_file_t
 * @param   max_age     time before item expires in the cache
 * @returns a cache_item_t pointer
 */ 
static cache_item_t new_cache_item(char *file_name, int max_age)
{
    unsigned char *file_buffer = NULL;
    int file_len = read_file_into_buf(file_name, &file_buffer);

    // this file expires at time = current_time + max_age (in clock ticks)
    clock_t exp_time = clock() + (CLOCKS_PER_SEC * max_age);

    cache_file_t new_file= { file_buffer, file_name, file_len, 
                              max_age, exp_time, 0 };

    cache_item_t new_item = malloc(sizeof(struct cache_item_t));
    new_item->file = new_file;
    new_item->next = NULL;

    return new_item;
}


/* free_cache_item()
 * @brief   given a malloc'd cache_item_t, frees it and all memory
 *          associated with its cache_file_t content.
 * @param   address of cache_item_t, a pointer, to free
 * @returns none
 * @note    if item is NULL, does nothing; assumes item was malloc'd
 */ 
static void free_cache_item(cache_item_t item)
{
        if (item == NULL)
            return;

        // free data from file buffer
        if ((item->file).data != NULL)
            free((item->file).data);
        
        if ((item->file).name != NULL)
            free((item->file).name);

        free(item);
        item = NULL;
}