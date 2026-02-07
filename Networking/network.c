#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
int* socketC;
void client();
void host();
int main() {
    int value;
    printf("Type 1 to run a server!, or type 0 to run a client\n");
    scanf("%d", &value);
    printf("Just read data\n");
    if(value > 0){
        printf("Starting server\n");
        host();
    }else{
        printf("Starting client\n");
        client();
    }
    return 0;
}
void client(){
     int status, valread, client_fd;
     struct sockaddr_in serv_addr;
     char* hello = "Hello from client";
     char buffer[1024] = {0};
     if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return;
     }

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <=0){
        printf("\n Invalid address/ Address not supported \n");
        return;
    }
    if((status = connect(client_fd, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr)))
            <= 0) {
        printf("\n Connection Failed \n");
        return;
    }
    send(client_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(client_fd,  buffer,
    1024-1);
    printf("%s\n", buffer);

    close(client_fd);
    return;
}
void host(){
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr*)&address,
        sizeof(address)) 
        <0){
            perror("bind failed. womp womp");
    }
    if(listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if((new_socket = accept(server_fd, (struct sockaddr*)&address,
        &addrlen)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024 - 1);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    close(new_socket);
    close(server_fd);
    return;
}
