#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Maximum application buffer
// Technically represents the maximum size of request
#define APP_MAX_BUFFER 1024
#define PORT 8080

int main(){
    // Define the server and client file descriptors
    int server_fd, client_fd;

    // define the socket address
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // define the application buffer where we receive the requests
    // data will be moved from receive buffer to here
    char buffer[APP_MAX_BUFFER] = {0};

    // create socket
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))==0){
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET; // ipv4
    address.sin_addr.s_addr = INADDR_ANY; // listen 0.0.0.0 interfaces
    address.sin_port = htons(PORT); // host to network order


    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // create the queues
    // listen for clients, with 10 backlog (10 connections in accept queue)
    if(listen(server_fd,10)<0){
        perror("Listen Failed");
        exit(EXIT_FAILURE);
    }

    //we loop forever
    while(1){
        printf("\nWaiting for a connection...\n");

        // Accept a client connection client_fd == connection
        // this blocks
        if((client_fd=accept(server_fd,(struct sockaddr *)&address, (socklen_t*)&address_len))<0){
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // read data from the OS receive buffer tot he application buffer
        // this is essentially reading the HTTP request
        // dont' bite more than you chew APP_MAX_BUFFER_SIZE
        read(client_fd, buffer, APP_MAX_BUFFER);
        printf("%s\n", buffer);

        // simulate slow processing request
        // wait for 6 seconds and then unblock 
        sleep(6);

        // we send the request by writing to the socket send buffer in the OS
        char *http_response = "HTTP/1.1 200 OK\n"
                              "Content-Type: text/plain\n"
                              "Content-Length: 13\n\n"
                              "Hello world!\n";
        // write to the socket
        // send queue OS
        write(client_fd, http_response, strlen(http_response));

        // close the client socket (terminate the TCP connection)
        close(client_fd);
    }

    return 0;
}