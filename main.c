/* 
 * MAIN.C
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include "sim_cache.h"

int main(int argc, char **argv)
{
    (void) argc;

    if (argc == 3) {
        int result = run_cache_sim(argv[1], atoi(argv[2]));
    }
    
    return 0;
}