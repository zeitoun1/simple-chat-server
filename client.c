#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "protocol.h"
#include "user.h"
#include "screen.h" 
#include "helper.h"


int main(int argc, char **argv) {

    // client socket configuration
    int client_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(client_soc == -1) {
        perror("socket");
        close(client_soc);
        exit(1);    
    }
    
    struct sockaddr_in server;
    struct addrinfo *result;

    getaddrinfo(host, NULL, NULL, &result);
    server.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
    freeaddrinfo(result);
    
    server.sin_family = AF_INET;
    memset(&(server.sin_zero), 0, 8);
    server.sin_port = htons(PORT);

    int connect_status = connect(client_soc, (struct sockaddr *)(&server), sizeof(struct sockaddr_in));

    if(connect_status == 0) {
        printf("Successfully connected\n");
    } else { // The user cannot conncet to the server, so exit
        perror("connect");
        exit(1); 
    }
    
    init_main_screen();
    
    fd_set openfds;
    fd_set readfds;
    FD_ZERO(&openfds);
    FD_ZERO(&readfds);

    FD_SET(fileno(stdin), &openfds); // to read messages from the user
    FD_SET(client_soc, &openfds); // to read messages from server

    char message_buf[MAXSIZE]; 
    char server_buf[MAXSIZE];
    char username[MAX_USERNAME];

    int username_len = init_username_screen(username, MAX_USERNAME);
    complete_write(client_soc, username, username_len + 1);
    

    init_message_box();
    init_text_box();
    
    while(1) {
        readfds = openfds;
        int num_ready = select(4, &readfds, NULL, NULL, NULL);

        if(num_ready == -1) {
            perror("select");
            exit(1);
        }
        
        if(FD_ISSET(client_soc, &readfds)) { // read new message from server
            int num_bytes_read = complete_read(client_soc, server_buf, MAXSIZE);
			

            if(num_bytes_read == -1) {  
                perror("read");
                close(client_soc);
                exit(1);
            }

            show_message(server_buf);
        }
        
        if(FD_ISSET(fileno(stdin), &readfds)) { // client has written a message
            int num_bytes_read = read_new_message(message_buf, MAXSIZE);
            if(num_bytes_read == -1) {
                perror("read");
                exit(1);
            } else if (num_bytes_read == 1) {
                printf("cannot send empty message\n");
                continue;
            }
            show_message(message_buf);
            complete_write(client_soc, message_buf, num_bytes_read);
			
        }
    
    }
      
        
}


 
