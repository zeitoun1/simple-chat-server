#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "helper.h"

/*
* Reads at most buf_size bytes from read_fd into buf, ensuring no data is unread
* On success returns the number of bytes read
* On failure (buffer overflow or read call failing) returns -1
*/

int complete_read(int read_fd, char *buf,  int buf_size) {
    int result = 0;
    int bytes_available;
    
    while(buf_size > 0) {
        ioctl(read_fd, FIONREAD, &bytes_available);

        if(bytes_available == 0) { // no more data to read
            return result;
        }

        int bytes_read = read(read_fd, buf, buf_size);

        if(bytes_read == -1) {
            return -1;
        }

        result += bytes_read;
        buf += bytes_read;
        buf_size -= bytes_read;
    }

    return result;
}


/*
* Writes exactly num_bytes from buf to write_fd ensuring no data is unwritten (assumes num_bytes <= sizeof(buf))
* On success returns the number of bytes read
* On failure returns -1
*/
int complete_write(int write_fd, char *buf, int num_bytes) {
    int result = 0;
    while(num_bytes > 0) {
        int bytes_written = write(write_fd, buf, num_bytes);

        if(bytes_written == -1) {
           return -1;
        } 

        result += bytes_written;
        buf += bytes_written;
        num_bytes -= bytes_written;
    }
    
    return result;
}


/*
* Returns the number of bytes in buf thar are part of complete (null terminated) messages
*/
int size_of_complete(char * buf, int num_bytes) {
    int last_null_terminator;
    for(int i = 0; i < num_bytes; i++) {
        if(buf[i] == '\0') {
            last_null_terminator = i;
        }
    }
    return last_null_terminator + 1;
}


/*
* end: represents the number of bytes in buf that are part of complete (null terminated) messages

* If the first num_bytes consist of complete messages i.e (end == num_bytes) it removes num_bytes data, otherwise 
* it shifts incomplete message to front of the buf and removes the rest of the data in the buffer.
* Returns number of non null bytes left in buffer (0 if buffer is empty after cleaning)
*/

int clean_buf(int end, int num_bytes, char *buf, int buf_size) {

    if (end == num_bytes)
    {
        memset(buf, '\0', num_bytes);
        return 0;
    }

    // shifting incomplete message to front of buffer
    memset(buf, '\0', end);
    memmove(buf, buf + end + 1, buf_size - end);
    memset(buf + buf_size - end, '\0', end);
    return buf_size - end;
}

/** 
 * Writes message in buff to all client file descriptors in openfds except the writingfd
 */
int write_to_clients(char *buf, fd_set *openfds, int num_bytes, int maxfd, int writingfd, int listen_soc, int buf_size) {
    int num_bytes_written;
    int end = size_of_complete(buf, num_bytes);
    for(int i = listen_soc + 1; i < maxfd + 1; i++) {
        if(i != writingfd && FD_ISSET(i, openfds)) {
            num_bytes_written = complete_write(i, buf, end);

            if(num_bytes_written == -1) { 
                fprintf(stderr, "error writing to client %d\n", i);
                exit(1);
            }
        }
        
    }
    clean_buf(end, num_bytes, buf, buf_size);
    return 0;
}
