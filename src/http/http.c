#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

#include "http.h"

int add_header(http_req* req, const char* name, const char* value)
{
    http_header* header;
    return 0;
}

http_req* parse_req(int* sock_fd) 
{
    char read_buf[1024];
    size_t buf_len = sizeof(read_buf);
    int bytes_read = read(*sock_fd, &read_buf, buf_len);
    if (bytes_read < 0) {
        return NULL;
    }
    char* tokens = strtok(read_buf, " ");
    int i = 0;
    char* params[3];
    while (tokens != NULL && i < 2) 
    {
        params[i] = tokens;
        tokens = strtok(NULL, " ");
        i++;
    }

    http_req* req = malloc(sizeof(http_req));
    char* method = params[0];
    if (strcmp(method, "GET")) 
    {
        req->method = HTTP_METHOD_GET;
    }
    req->path = params[1];

    return req;
}

const char* resolve_resp_code(int code) {
    switch (code) {
        case HTTP_RESP_OK:
            return "200 OK";
        case HTTP_RESP_BAD_REQUEST:
            return "400 Bad Request";
        case HTTP_RESP_NOT_FOUND:
            return "404 Not Found";
        case HTTP_RESP_INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        default:
            return NULL;
    }
}

int write_resp(int* sock_fd, int status, char* headers, char* body)
{
    char response[1024] = "";

    const char* status_str = resolve_resp_code(status);
    sprintf(response, "HTTP/1.1 %d %s\r\n", status, status_str);

    strcat(response, headers);
    strcat(response, body);

    int bytes_written = write(*sock_fd, response, sizeof(response));

    return 0;
}
