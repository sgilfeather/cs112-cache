/* 
 * SIM_CACHE.C
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include "sim_cache.h"

/*** HELPER FUNCS ***/

// finds the first backslash, '\', in a cache command
static inline int find_char(char *string, int str_len, char c);

static inline char *get_substr(char *string, int a, int b);

static inline char *generate_output_name(char *string);

static void wait_cmd(int time_to_wait);

/* init_cache_sim()
 * @brief   given an input file of commands, run caching sim with commands
 * @param   cmd_file_name   name of command file to read from
 * @param   cache_size      size of cache, specified at runtime
 * @param   buffer          buffer to put file data in
 * @returns an instance of C_T representing the given cache
 * 
 * @note if read fails, returns NULL  
 */ 
C_T init_cache_sim(char *cmd_file_name, int cache_size, 
                   unsigned char **buffer)
{
    if (cmd_file_name == NULL || cache_size < 1)
        return NULL;

    int result = read_file_into_buf(cmd_file_name, buffer);
    if (result == -1) // if file couldn't be read properly, return
        return NULL;
     
    C_T our_cache = create_cache(cache_size); // malloc'd
    return our_cache;
}


/* run_cache_sim()
 * @brief   run caching sim with given input file and generated cache
 * @param   cmd_file_name   name of command file to read from
 * @param   cache_size      size of cache, specified at runtime
 * @returns 0 if run successfully, 1 if an error is encountered
 * @note    
 */ 
int run_cache_sim(char *cmd_file_name, int cache_size)
{
    unsigned char *cmd_file = NULL;
    char *line = NULL;
    char *file_name = NULL;
    int max_age = -1;

    C_T cache = init_cache_sim(cmd_file_name, cache_size, &cmd_file);

    line = strtok((char *)cmd_file, "\n");
    while (line != NULL) {
        max_age = extract_command(line, strlen(line), &file_name);
        // if (file_name == NULL && max_age != -1) {
        //     // printf("WAIT: %i\n", max_age);
        //     wait_cmd(max_age);
        // }
        // else 
        if (file_name != NULL && max_age != -1) {
            // PUT 
            // printf("PUT: %s, %i\n", file_name, max_age);
            put_cmd(cache, file_name, max_age);
        }
        else if (file_name != NULL) {
            // printf("GET: %s\n", file_name);
            get_cmd(cache, file_name);
        }

        // print_cache(cache);
        line = strtok(NULL, "\n"); // get the next line
         // reset to default values; cache item keeps track of file_name
        max_age = -1;
        file_name = NULL;
    }

    if (file_name != NULL) {
        free(file_name);
    }

    free_cache(cache);

    return 0;
}

/* wait_cmd()
 *
 *
 * 
 */ 
static void wait_cmd(int time_to_wait) {
    clock_t now = clock();
    clock_t then = now + (CLOCKS_PER_SEC * time_to_wait);
    while (now != then){
        now = clock();
    }
}

/* put_cmd()
 * @brief   executes PUT <file> <age>. evicts a stale files from the cache,
 *          if necessary, then stores new file 
 * @param   cache   C_T cache instance to work with
 * @param   file_name   name of file to store
 * @param   max_age     maximum age (in sec) for file to stay fresh in cache
 * @returns none
 * 
 * @note    eviction policy: if all files have been accessed before, evict the
 *          least-recently accessed file; otherwise, evict the oldest file
 */
void put_cmd(C_T cache, char *file_name, int max_age)
{
    int size = size_of_cache(cache);
    int cap = cap_of_cache(cache);

    // check if it already exists in cache
    cache_file_t our_file = retrieve_file_struct(cache, file_name);

    // if file doesn't exist (NAME IS NULL), add it to the cache!
    if (our_file.name == NULL) {
        if (size >= cap) {
            cache = (C_T)evict_one(cache);
        }
        // must create unique, new name string for new item!

        cache = (C_T)push_back_cache(cache, file_name, max_age);
    } else {
        // else, update content for an existing file
        cache_file_t new_file = retrieve_file_struct(cache, file_name);
        new_file.max_age = max_age; // update max age if changed
        cache = (C_T)update_item_cache(cache, file_name, new_file);
    }
}


/* get_cmd()
 * @brief   executes <GET> <file>. if file exists in cache, get its data from
 *          cache and write to an output file named <file>_output<ext>.
 * @param   cache   C_T cache instance to work with
 * @param   file_name   name of file to get
 * @returns none
 * 
 * @note    if file exists, but age has timed out, re-”get” file data
 *          from source and update cache content before writing to output
 *          file. if file doesn’t exist in cache, do nothing.
 */
void get_cmd(C_T cache, char *file_name)
{
    cache_file_t our_file = retrieve_file_struct(cache, file_name);
    printf("asked for %s, got %s\n", file_name, our_file.name);
    if (our_file.name != NULL) { // if file isn't NULL_FILE
        clock_t now = clock();

        // if file is expired, "re-get" and update file content 
        if (our_file.expiration <= now) {
            cache = (C_T)remove_file_cache(cache, file_name); // frees name
            cache = (C_T)push_back_cache(cache, file_name, our_file.max_age);
        }
        // else, just update item
        else { 
            cache = (C_T)update_item_cache(cache, file_name, our_file);
        }

        char *new_name = generate_output_name(file_name); // malloc'd
        write_buf_into_file(new_name, our_file.data, our_file.len);
        free(new_name);
    }
    else {
        printf("we couldn't find %s in cache\n", file_name);
    }

    // if file is not in cache, don't do anything!
}


/* extract_command()
 * @brief   given a command string, checks whether it's a proper cache
 *          command; if so, returns its content
 * @param   string  string to parse
 * @param   str_len length of string to parse
 * @param   file_name   pointer to hold file's name,
 * @returns             max age of file, if applicable, from command
 * 
 * @note    for PUT command, max_age is written with data;
 *          for GET command, max_age is returned as -1;
 *          for invalid string, max_age is returned as -1 and file_name is
 *          set to NULL
 */ 
int extract_command(char *string, int str_len, char **file_name)
{
    int max_age;
    *file_name = NULL;

    if (str_len < 6) { // a command should have minimally 6 chars
        return -1;
    }

    // // if command is WAIT: x (sec), return sec as max_age, NULL filename
    // if (strncmp("WAIT: ", string, 6) == 0) {
    //     file_name = NULL;
    //     char *sec = get_substr(string, 6, str_len);
    //     int age = atoi(sec);
    //     free(sec);
    //     return age;
    // }

    // if command is "GET: "
    if (strncmp("GET: ", string, 5) == 0) { 
        // copy name from content string into file_name 
        *file_name = get_substr(string, 5, str_len); // malloc'd!
        return -1; 
    } 
    // if command is "PUT: "
    else if (strncmp("PUT: ", string, 5) == 0) {
        int slash_ind = find_char(string, str_len, '\\');
        if (slash_ind == -1 || slash_ind == 5) 
            return -1;  // if no slash or no file name, command is invalid

        // pointer to 'start' of MAX_AGE in string
        char *age_content = get_substr(string, slash_ind + 1, str_len);

        if (strncmp("MAX-AGE: ", age_content, 9) == 0) {
            int age_ind = slash_ind + 10; // ind of num after "\MAX-AGE: "

            char *age_str = get_substr(string, age_ind, str_len);

            if (age_str != NULL) {
                max_age = atoi(age_str);
                // copy name from content string into file_name
                *file_name = get_substr(string, 5, slash_ind);
            }
            else {
                max_age = -1;
            }

            free(age_str);
            free(age_content);
        }
    }

    // if command doesn't start with "GET: " or "PUT: ", it's invalid
    return max_age; // if invalid, will return *file_name as NULL
}


/* get_substr
 * @brief   Given a C string, return the substring from [a, b)– starting
 *          at a, ending before b. 
 * @param   string, str_len the given string of length str_len
 * @param   a, b            range of substring: [a, b)
 * @returns the substring found
 * 
 * @note    if a or b is out of range, or b - a > str_len, returns NULL 
 * @note    assumes string is length str_len 
 * @note    string is malloc'd on the heap and must be freed.
 */ 
static inline char *get_substr(char *string, int a, int b)
{
    int str_len = strlen(string);

    if (a < 0 || b > str_len || (b - a) > str_len)
        return NULL;

    int len = b - a;
    char *substr = (char *)malloc(len + 1);

    // offset to start of substr within string
    memset(substr, '\0', len + 1);
    strncpy(substr, string + a, len);

    return substr;
}


/* find_char()
 * @brief   finds first index of a given char, c, in the string
 * @param   string, str_len string with a given length
 * @param   c       char to find!
 * @returns index of given char, or '-1' if not found.
 */ 
static inline int find_char(char *string, int str_len, char c)
{
    int i;
    for (i = 0; i < str_len; i++) {
        if (string[i] == c)
            return i;
    }
    return -1;
}


/* generate_output_name()
 * @brief   given a file named <file><optional extention>, return the name
 *          "<file>_output<optional extention>".
 * @param   name to output-ify
 * @returns new output name
 * @note    new output name is malloc'd and must be freed.
 */
static inline char *generate_output_name(char *string)
{
    if (string == NULL)
        return NULL;

    int str_len = strlen(string);
    int dot_ind = find_char(string, str_len, '.');
    char *out = "_output";
    int len_out = strlen(out);

    // add extra character for null terminator
    char *new_name = malloc(sizeof(char) * (str_len + len_out + 1));

    if (dot_ind == -1 || dot_ind >= str_len) {
        strncpy(new_name, string, str_len);
        strncpy(new_name + str_len, out, len_out);
        new_name[str_len + len_out] = '\0'; // set null terminator
    }
    else {
        char *ext = get_substr(string, dot_ind, str_len);

        strncpy(new_name, string, dot_ind); // copy over text before dot
        strncat(new_name, out, strlen(out));
        strncat(new_name, ext, strlen(ext));

        free(ext);
    }

    return new_name;
}