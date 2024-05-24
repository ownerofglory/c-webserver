#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <strings.h>
#include <pthread.h>

#include "fs.h"
#include "http.h"
#include "hash.h"

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
            http_req* req = parse_req(&sock_conn);
            if (req == NULL)
            {
                write_resp(&sock_conn, HTTP_RESP_BAD_REQUEST, "\r\n\r\n", "");
                goto rw_loop;
            }
            
            char* server_path = "./site";
            char* file_name = req->path; 
            if(strcmp(req->path, "/") == 0)
            {
                file_name = "/index.html";
            }

            char str_buf[128] = "";
            strcat(str_buf, server_path);
            strcat(str_buf, file_name);

            int f_size = file_size(str_buf);
            if (f_size < 0) 
            {
                printf("Unable to determine file size: %d\n", f_size);
                write_resp(&sock_conn, HTTP_RESP_NOT_FOUND, "\r\n\r\n", "");        
                goto rw_loop;
            }

            char file_buf[f_size];
            int result = read_file(str_buf, file_buf, (size_t) f_size + 1);
            if (result < 0) 
            {
                switch(result) 
                {
                    case ERR_FILE_NOT_FOUND:
                        write_resp(&sock_conn, HTTP_RESP_NOT_FOUND, "\r\n\r\n", "");
                        break;
                    case ERR_READ_FAIL:
                    case ERR_BUF_TOO_SMALL:
                        write_resp(&sock_conn, HTTP_RESP_INTERNAL_SERVER_ERROR, "\r\n\r\n", "");
                        break;
                }
                goto rw_loop;
            }

            int ok = write_resp(&sock_conn, HTTP_RESP_OK, "Content-Type: text/html\r\n\r\n", file_buf);

            goto rw_loop;
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
