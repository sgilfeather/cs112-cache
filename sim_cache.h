/* 
 * SIM_CACHE.H
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cache.h"
#include "file_sys.h"

// checks whether a string is a valid command and gets data from it
int extract_command(char *string, int str_len, char **file_name);

// initiates sim by generating cache structure and opening command file
C_T init_cache_sim(char *cmd_file_name, int cache_size, 
                   unsigned char **buffer);

// runs caching sim, parsing the command file and running its commands
int run_cache_sim(char *cmd_file_name, int cache_size);

/*** CACHE COMMANDS ***/

void put_cmd(C_T cache, char *file_name, int max_age); // performs PUT command

void get_cmd(C_T cache, char *file_name); // performs GET command

void evict(C_T cache, char *file_name);


/*** CACHE HELPER FUNCS ***/

// returns num of PUT files that have been retrieved (GET) at least once
uint32_t num_retrieved();