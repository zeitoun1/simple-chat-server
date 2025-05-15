#ifndef HELPER_H
#define HELPER_H


#include <sys/select.h>

int complete_read(int read_fd, char *buf,  int buf_size);
int complete_write(int write_fd, char *buf, int num_bytes);
int size_of_complete(char * buf, int num_bytes);
int clean_buf(int end, int num_bytes, char *buf, int buf_size);
int write_to_clients(char *buf, fd_set *openfds, int num_bytes, int maxfd, int writingfd, int listen_soc, int buf_size);

#endif
