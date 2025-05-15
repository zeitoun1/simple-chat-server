#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include "protocol.h"
#include "user.h"
#include "helper.h"



int listen_soc;
User_T users_arr[MAX_USERS]; // starts indexing from 4 (first 4 elements are empty) and index corresponds to fd of the user


int main(int argc, char **argv) {
    fd_set openfds;
    fd_set readfds;
    FD_ZERO(&openfds);
    FD_ZERO(&readfds);
    
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
            struct sockaddr_in client_addr;
            client_addr.sin_family = AF_INET;
            unsigned len = sizeof(struct sockaddr_in);
            int client_soc = accept(listen_soc, (struct sockaddr *) &client_addr, &len);

            if(client_soc == -1) {
                perror("accept");
                close(listen_soc);
                exit(1);
            }
            
            users_arr[client_soc].username_is_sent = 0;
            users_arr[client_soc].read_start = users_arr[client_soc].buf;

            printf("[%d] A new client has connected\n", client_soc);

            FD_SET(client_soc, &openfds);
            if(client_soc > max_fd) {
                max_fd = client_soc;
            }

            
        }

        // Read messages/username from clients
        for(int i = listen_soc + 1; i < max_fd + 1; i++) {
            if(FD_ISSET(i, &readfds)) {
                if(users_arr[i].username_is_sent) { // read message
                    int num_bytes_read = complete_read(i, users_arr[i].read_start, MAXSIZE);
                    if(num_bytes_read == -1) {
                        perror("read");
                        exit(1);
                    }
                    int next_read_pos = write_to_clients(users_arr[i].buf, &openfds, num_bytes_read, max_fd, i, listen_soc, MAXSIZE);
                    users_arr[i].read_start = users_arr[i].buf +  next_read_pos;
                }else { // read username
                    int num_bytes_read = complete_read(i, users_arr[i].username, MAX_USERNAME);
                    if(num_bytes_read == -1) {
                        perror("read");
                        exit(1);
                    }
                    users_arr[i].username_is_sent = 1;
                }
            }
        }


    }

}
