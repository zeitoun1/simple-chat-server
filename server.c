#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include "protocol.h"



int listen_soc;

/** 
 * Writes message in buff to all client file descriptors in openfds except the writingfd
 */
void write_to_clients(char *buf, fd_set *openfds, int num_bytes, int maxfd, int writingfd) {
    int num_bytes_written;
    for(int i = listen_soc + 1; i < maxfd + 1; i++) {
        if(i != writingfd && FD_ISSET(i, openfds)) {
            num_bytes_written = write(i, buf, num_bytes);
            if(num_bytes_written < num_bytes) { 
                fprintf(stderr, "message wasn't fully written to %d\n", i);
            }
        }
        
        // some sort of error handling should be done here
    }
}

int main(int argc, char **argv) {
    
    fd_set openfds;
    fd_set readfds;
    FD_ZERO(&openfds);
    FD_ZERO(&readfds);
    char buf[MAXSIZE];
    
    // server socket configuration
    listen_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_soc == -1) {
        perror("socket");
        close(listen_soc);
        exit(1);
    }
    FD_SET(listen_soc, &openfds);
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);
    
    if(bind(listen_soc, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        close(listen_soc);
        exit(1);
    }
    
    if(listen(listen_soc, 5) == -1) {
        perror("listen");
        close(listen_soc);
        exit(1);
    }
    
    printf("Server waiting for connections...\n");
    int max_fd = listen_soc;
    
    while(1) {

        readfds = openfds;
        int num_ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if(num_ready == -1) {
            perror("select");
            exit(1);
        }
        
        // accepting incoming connection (if there are any)
        if(FD_ISSET(listen_soc, &readfds)) {
            printf("%d", FD_ISSET(listen_soc, &readfds));
            struct sockaddr_in client_addr;
            client_addr.sin_family = AF_INET;
            unsigned len = sizeof(struct sockaddr_in);
            int client_soc = accept(listen_soc, (struct sockaddr *) &client_addr, &len);
            
            if(client_soc == -1) {
                perror("accept");
                close(listen_soc);
                exit(1);
            }
            printf("[%d] A new client has connected\n", client_soc);

            FD_SET(client_soc, &openfds);
            if(client_soc > max_fd) {
                max_fd = client_soc;
            }
        }

        // Read messages from clients (if there are any) and send them to all clients
        for(int i = listen_soc + 1; i < max_fd + 1; i++) {
            if(FD_ISSET(i, &readfds)) {
                int num_bytes_read = read(i, buf, MAXSIZE - 1);
                if(num_bytes_read == -1) {
                    perror("read");
                    exit(1);
                }
                write_to_clients(buf, &openfds, num_bytes_read, max_fd, i);
            }
        }


    }

}