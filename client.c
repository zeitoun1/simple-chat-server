#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 54321

int main(int argc, char **argv) {
    int client_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(client_soc == -1) {
        perror("socket");
        close(client_soc);
        exit(1);
    }
    
    struct sockaddr_in server;
    struct addrinfo *result;

    getaddrinfo("zeitoun-Yoga-7-16IAH7", NULL, NULL, &result);
    server.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
    freeaddrinfo(result);

    server.sin_family = AF_INET;
    memset(&(server.sin_zero), 0, 8);
    server.sin_port = htons(PORT);

    int connect_status = connect(client_soc, (struct sockaddr *)(&server), sizeof(struct sockaddr_in));

    if(connect_status == 0) {
        printf("Successfully connected\n");
    } else {
        printf("failed to connect");
    }


}   