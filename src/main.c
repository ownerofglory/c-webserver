#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "fs.h"

#define PORT 8080
#define TRUE 1
#define FALSE 0
#define READ_BUF_SIZE 1024

int main(int arcg, char** args) 
{
    // int     socket(namespace, style, protocol);
    int sock_fd = socket (PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
    {
        perror("Cannot create socket file descriptor\n");
        exit(1);
    }

    struct sockaddr_in addr;
    socklen_t socklen = sizeof(addr);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    printf("Binding port %d to socket(%d)\n", PORT, sock_fd);
    // socket, socket_addr, socket_len
    int bound = bind(sock_fd, (struct sockaddr*) &addr, socklen);
    if (bound < 0) 
    {
        perror("Cannot bind socket to port");
        exit(1);
    }

    printf("Listeing on port %d\n", PORT);
    int listening = listen(sock_fd, 4);
    if (listening < 0) 
    {
        perror("Cannot listen to port\n");
        exit(1);
    }

    printf("Starting server loop\n");
outer_loop:
    while (TRUE) 
    {
        int sock_conn = accept(sock_fd, (struct sockaddr*) &addr, &socklen);
        if (sock_conn < 0) 
        {
            perror("Error when accepting a socket connection\n");
            exit(1);
        }
        printf("Incoming socket connection socket(%d)\n", sock_conn);

        while (TRUE) 
        {
            char read_buf[READ_BUF_SIZE];
            size_t buf_len = sizeof(read_buf);
            int bytes_read = read(sock_conn, &read_buf, buf_len);
            if (bytes_read <= 0) 
            {
                goto rw_loop;
            }

            printf("%s\n", read_buf);

            char* message = read_file("./site/index.html");
            if (message == NULL) 
            {
                goto rw_loop;
            }
            int bytes_written = write(sock_conn, message, sizeof(message));
            free(message);
            if (bytes_written <= 0) 
            {
                goto rw_loop;
            }
        }
        rw_loop:
        printf("Closing socket(%d) connection\n", sock_conn);

        close(sock_conn);
    }
    
    printf("Closing server socket(%d)\n", sock_fd);
    close (sock_fd);

    printf("Server shutdown\n");

    return 0;
}
