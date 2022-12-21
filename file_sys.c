/* 
 * FILESYS.C
 * A0
 * 
 * @author Skylar Gilfeather
 * @date CS112, Fall 2022
 * 
 */ 

#include "file_sys.h"

/* read_file_into_buf()
 * @brief   reads contents of the given file into a buffer
 * @param   file_name   name of file to read
 * @param   buffer      address of buffer to allocate for file data
 * @returns number of bytes read into buffer (file size)
 * @note    if file can't be opened, or read fails, returns -1
 */ 
int read_file_into_buf(char *file_name, unsigned char **buffer)
{
    struct stat st;

    int fildes = open(file_name, O_RDONLY);
    if (fildes == -1) {// if open fails (file is not found)
        return -1;
    }

    stat(file_name, &st);
    if (st.st_size < 0) {
        printf("couldnt' stat the file\n");
        return -1;
    }
    *buffer = malloc(st.st_size);
    memset(*buffer, 0, st.st_size);    

    // if we couldn't properly allocate buffer
    if (buffer == NULL) {
        close(fildes);
        return -1;
    }

    int bytes_read = read(fildes, *buffer, st.st_size);
    close(fildes);

    if (bytes_read < st.st_size) { // if read does not read entire file
        return -1;
    }

    return st.st_size; // length of file, in bytes
}


/* write_buf_into_file()
 * @brief   given a data buffer of size buf_len, write the data into a
 *          new file called "file_name"
 * @param   file_name   name of file to be created
 * @param   buffer      buffer of bytes to write into file
 * @param   buf_len     number of bytes
 * @returns none, but creates a new file in the current directory
 * 
 * @note    returns -1 if buffer is invalid; 
 * @note    buf_len must be >= 0
 */
// write buffer data out into file; returns num of bytes written
int write_buf_into_file(char *file_name, unsigned char *buffer, uint32_t buf_len)
{
    if (buffer == NULL) { // cannot write from invalid buffer
        // printf("buffer was NULL.\n");
        return -1;
    }

    int fildes = open(file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (fildes == -1) { // if open fails (file is not found) 
        // printf("we couldn't open the file\n");
        return -1;
    }

    int bytes_written = write(fildes, buffer, buf_len);
    close(fildes);

    if (bytes_written < (int)buf_len) { // if write was not successful
        // printf("we didn't write the entire file buffer\n");
        return -1;
    }

    return bytes_written;
}


/* delete_file()
 * @brief   wrapper function to delete the given file, if it exists
 * @param   name of file to delete
 * @returns none
 */ 
void delete_file(char *file_name)
{
    unlink(file_name);
}