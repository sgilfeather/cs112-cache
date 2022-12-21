/* 
 * TEST_CACHE.C
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include "test_cache.h"

#define NUM_TESTS 6


/* run_tests()
 * @brief   for each program-testing function within the given tests_t suite,
 *          run the function and report its results
 * @param   array of function pointers to tester functions
 */ 
void run_tests(int (**functions)())
{
    if (functions == NULL)
        return;  

    for (int i = 0; i < NUM_TESTS; i++) {
        fprintf(stderr, "TEST [%u]:\n", i);
        int result = (*functions[i])(); // call the function at index i

        switch (result) {
            case 1:
                fprintf(stderr, "\tSUCCESS!\n\n");
                break;

            case 0:
                fprintf(stderr, "\tFAILED.\n\n");
                break;

            case -1:
                fprintf(stderr, "\tERROR: FAULTED.\n\n");
                break;
        }
    }
}


/*** CACHE UTIL TESTS ***/


/* test_create_free_cache()
 *
 *
 * 
 */ 
int test_create_free_cache()
{
    C_T cache_0 = create_cache(0);   
    C_T cache_1 = create_cache(8);   
    C_T cache_2 = create_cache(-1);   

    if (cache_0 != NULL) {
        fprintf(stderr, "\t\tERROR: Poorly-sized caches not NULL.\n");
        return 0;
    }

    if (!(cap_of_cache(cache_1) == 8 && size_of_cache(cache_1) == 0)) {
        fprintf(stderr, "\t\tERROR: Cache has incorrect cap or size.\n");
        return 0;
    }

    free_cache(cache_0);
    fprintf(stderr, "\tFreed cache of size 0.\n");
    free_cache(cache_1);
    fprintf(stderr, "\tFreed cache of size 8.\n");
    free_cache(cache_2);
    fprintf(stderr, "\t'Freed' cache of size -1.\n");

    return 1;    
}


/* test_push_back_cache
 *
 *
 * 
 */ 
int test_push_back_cache()
{
    C_T cache_0 = create_cache(12);

    cache_0 = (C_T)push_back_cache(cache_0, "file_a", 60);
    cache_0 = (C_T)push_back_cache(cache_0, "file_b", 100);

    if (size_of_cache(cache_0) != 2) {
        fprintf(stderr, "\tERROR: File not added to cache properly.\n");
        return 0;
    }

    cache_file_t file_a = retrieve_file_struct(cache_0, "file_a");
    cache_file_t file_b = retrieve_file_struct(cache_0, "file_b");
    print_file_struct(file_a);
    print_file_struct(file_b);

    free(cache_0);
    return 1;    
}


/* test_pop_front_cache
 *
 *
 * 
 */ 
int test_pop_front_cache()
{
    C_T cache_0 = create_cache(4);
    // C_T cache_1 = create_cache(40);

    cache_0 = (C_T)push_back_cache(cache_0, "file_a", 60);
    cache_0 = (C_T)push_back_cache(cache_0, "file_b", 100);

    print_cache(cache_0);

    cache_0 = (C_T)remove_file_cache(cache_0, "file_a");

    if (size_of_cache(cache_0) != 1) {
        fprintf(stderr, "\tERROR: File not popped from cache properly.\n");
    }

    fprintf(stderr, "\tShould retrieve a NULL_FILE: file_a isn't in cache.\n");
    cache_file_t file_a = retrieve_file_struct(cache_0, "file_a");
    print_file_struct(file_a); // should be NULL_FILE: not found

    free(cache_0);
    return 1;    
}


/*** FILE UTIL TESTS ***/


int test_read_file_to_buf()
{
    unsigned char *buffer;
    int result = read_file_into_buf("test10.txt", &buffer);

    if (result == -1 || buffer == NULL)
        return 0;

    free(buffer);
    return 1;
}

int test_write_buf_to_file()
{
    unsigned char *buffer = NULL;
    unsigned char *buffer_0 = NULL;
    int num_bytes = read_file_into_buf("dga.jpg", &buffer);

    int num_written = write_buf_into_file("dga_output.jpg", buffer, num_bytes);
    if (num_written == -1)
        return 0; // confirm that we wrote the entire buffer into file

    printf("We wrote %i bytes, after reading %i bytes!\n", 
            num_written, num_bytes);
    num_bytes = read_file_into_buf("dga.jpg", &buffer_0);

    for (int i = 0; i < num_bytes; i++){
        if (buffer[i] != buffer_0[i]) {
            fprintf(stderr, "\tBuffers are different at byte #%i.\n", i);
            return 0;
        }
    }

    fprintf(stderr, "\tBuffers are equal!\n");

    return 1;
}


/*** SIM CACHE TESTS ***/

/* NOTE: include get_substr() in sim_cache.h and remove its 'inline'
 * descriptor in order to test it publicly.
 */
// int test_get_substr()
// {
//     char *tester = "abcdefghijklmnop";
//     char *substr = get_substr(tester, strlen(tester), 7, strlen(tester));
//     if (strcmp(substr, "hijklmnop") == 0)
//         return 1;
//     else {
//         fprintf(stderr, "\tWe got substr: %s\n", substr);
//         return 0;
//     }
// }

int test_extract_command_goods()
{
    char *good_0 = "GET: hello.txt";
    char *good_1 = "PUT: hello.txt\\MAX-AGE: 60";
    char *good_2 = "PUT: hello.txt\\MAX-AGE: 0";

    char *strs [3] = { good_0, good_1, good_2 };

    char *file_name = NULL;
    int max_age = -1;
    for (int i = 0; i < 3; i++) {
        max_age = extract_command(strs[i], strlen(strs[i]), &file_name);

        if (file_name == NULL) {
            fprintf(stderr, "\tIssue extracting command from %s.\n", strs[i]);
            return 0;
        }

        if (strcmp(file_name, "hello.txt") != 0) {
            fprintf(stderr, "\tFile name is %s\n", file_name);
            return 0;
        }

        if (max_age != -1) {
            fprintf(stderr, "\tPUT: Age is %i\n", max_age);
        }
    }
    
    if (file_name != NULL) {
        free(file_name);
    }
    return 1;
}


int test_extract_command_bads()
{
    char *bad_0 = "This is not a command.";
    char *bad_1 = "";
    char *bad_2 = NULL;
    char *bad_3 = "GET: ";
    char *bad_4 = "PUT: a.txt\\MAX-AGE: ";
    char *bad_5 = "PUT: a.txt\\MAX-AGE: NaN";
    char *bad_6 = "PUT: hasaslash\\.out\\MAX-AGE: 0";

    char *strs [7] = { bad_0, bad_1, bad_2, bad_3, bad_4, bad_5, bad_6 };

    char *file_name = NULL;
    int max_age = -1;
    for (int i = 0; i < 7; i++) {

        if (strs[i] != NULL)
            max_age = extract_command(strs[i], strlen(strs[i]), &file_name);
        else
            max_age = extract_command(strs[i], 0, &file_name);

        if (file_name == NULL) {
            fprintf(stderr, "\t%s: not a command.\n", strs[i]);
        }

        if (max_age == -1) {
            fprintf(stderr, "\t%s: badly formatted age.\n", strs[i]);
        }
    }
    fprintf(stderr, "\n");
    return 1;
}


// uses private member struct, cache_item_t
// int test_find_in_cache()
// {
//     C_T cache = create_cache(12);
//     cache = (C_T)push_back_cache(cache, "hello", 10);
//     cache = (C_T)push_back_cache(cache, "babygirl", 10);
//     cache = (C_T)push_back_cache(cache, "yuh.txt", 10);


//     cache_item_t item = NULL;
//     int ind = find_in_cache(cache, "hello", &item);
//     fprintf(stderr, "We found %s at %i\n", (item->file).name, ind);
//     return 1;
// }


/* test_read_write_file()
 * @brief   reads a file into a buffer, and outputs buffer into new file
 */ 
int test_read_write_file()
{
    unsigned char *buffer = NULL;
    char *file_name = "dga.jpg";
    char *out_name = "dga_output.jpg";

    int numRead = read_file_into_buf(file_name, &buffer);
    assert(numRead != -1);
    printf("We read %i bytes into buffer.\n", numRead);

    int numWritten = write_buf_into_file(out_name, buffer, numRead);
    printf("We wrote %i bytes of buffer into file.\n", numWritten);
    assert(numWritten != -1 && numWritten == numRead);

    return 1;
}

/* main
 *
 *
 * 
 */ 
int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    // define, and initialize, an array of function pointers for each test
    int (*functions[NUM_TESTS])() = { &test_create_free_cache,
                              &test_push_back_cache,
                              // &test_find_in_cache,
                              &test_read_file_to_buf,
                              &test_write_buf_to_file,
                              // &test_get_substr,
                              &test_read_write_file,
                              &test_extract_command_goods,
                              // &test_extract_command_bads
                                    };

    fprintf(stderr, "STARTING UNIT TESTS...%u TOTAL.\n\n", NUM_TESTS);
    run_tests(functions);

    return 0;
}

