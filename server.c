#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#define MAXSIZE 80


/** 
* Writes message in buff to all client file descriptors in writefds
*/
int write_to_clients(char *buff, fd_set writefds) {

}

int main(int argc, char **argv) {

    char buff[MAXSIZE];

    // server socket configuration
    int listen_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_soc == -1) {
        perror("socket");
        close(listen_soc);
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54321);
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

    fd_set readfds;
    FD_ZERO(&readfds);
    int max_fd = 0;
    int result = fork();
    // accepting connections and communication between server

    if(result == 0) { // process for reading messages from clients and writing them to the rest of the clients
        if(close(listen_soc) == -1) {
            perror("close");
            exit(1);
        }

        int num_ready = select(max_fd, &readfds, NULL, NULL, NULL);
        if(num_ready == -1) {
            perror("select");
            exit(1);
        }

        for(int i = 0; i < max_fd; i++) {
            if(FD_ISSET(i, &readfds)) {
                int num_bytes_read = read(i, buff, MAXSIZE);
                if(num_bytes_read == -1) {
                    perror("read");
                    exit(1);
                }
                write_to_clients();
            }
        }

    }else { // process for accepting connections
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        int len = sizeof(struct sockaddr_in);
        int client_soc = accept(listen_soc, (struct sockaddr_in *) &client_addr, &len);
    
        if(client_soc == -1) {
            perror("accept");
            close(listen_soc);
            exit(1);
        }

        FD_SET(client_soc, &readfds);
        if(client_soc > max_fd) {
            max_fd = client_soc;
        }
        
    }
    

}   