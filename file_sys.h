/* 
 * FILESYS.H
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// reads an entire file into a malloc'd buffer; returns num of bytes read 
int read_file_into_buf(char *file_name, unsigned char **buffer);

// write buffer data out into file; returns num of bytes written
int write_buf_into_file(char *file_name, unsigned char *buffer, 
                        uint32_t buf_len);

// removes a file from the given directory (if it exists)
void delete_file(char *file_name);