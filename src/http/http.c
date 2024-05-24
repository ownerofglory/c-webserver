#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "http.h"
#include "hash.h"

void trim_str(char *str) {

    while (isspace((unsigned char)str[0]))
    {
        memmove(str, str + 1, strlen(str));   
    }

    while (isspace((unsigned char)str[strlen(str) - 1]))
    {
        str[strlen(str) - 1] = '\0';
    }
}

int add_header(http_req* req, const char* header)
{
    size_t header_size = strlen(header);
    char* h = malloc(header_size);

    strcpy(h, header);

    char* h_name = strtok(h, ":");
    char* h_val = strtok(NULL, ":");
    trim_str(h_val);

    http_header http_h;
    http_h.name = h_name;
    http_h.value = h_val;

    ht_add_value(req->headers, h_name, h_val);

    free(h);

    return 0;
}

int parse_req_method(http_req* req, char* method_s)
{
    if (strcmp(method_s, "GET")) 
    {
        req->method = HTTP_METHOD_GET;
    } else if (strcmp(method_s, "POST"))
    {
        req->method = HTTP_METHOD_POST;
    } else if (strcmp(method_s, "PUT"))
    {
        req->method = HTTP_METHOD_PUT;
    } else if (strcmp(method_s, "DELETE"))
    {
        req->method = HTTP_METHOD_DELETE;
    } else if (strcmp(method_s, "HEAD"))
    {
        req->method = HTTP_METHOD_HEAD;
    } else 
    {
        return 1;
    }
    return 0;
}

http_req* parse_req(int* sock_fd) 
{
    char read_buf[SERVER_READ_BUF_SIZE];
    size_t buf_len = sizeof(read_buf);
    int bytes_read = read(*
    sock_fd, &read_buf, buf_len);
    if (bytes_read < 0) {
        return NULL;
    }

    // split request headers from body
    const char* delimiter = "\r\n\r\n";
    char* headers_end = strstr(read_buf, delimiter);
    if (headers_end == NULL) {
        return NULL;
    }

    // read request headers part
    size_t headers_size = headers_end - read_buf + 4;
    char* req_headers = malloc(headers_size + 1);
    if (req_headers == NULL) {
        return NULL;
    }

    strncpy(req_headers, read_buf, headers_size);
    req_headers[headers_size] = '\0';

    // read request line [METHOD] [PATH] [VERSION]
    char* req_line_end =  strstr(req_headers, "\r\n");
    if (req_line_end == NULL) {
        return NULL;
    }

    size_t req_line_size = req_line_end - req_headers;
    char* req_line = malloc(req_line_size + 1);
    if (req_line == NULL) {
        return NULL;
    }

    strncpy(req_line, req_headers, req_line_size);
    req_line[req_line_size] = '\0';
    
    // parse request line
    char* tokens = strtok(req_line, " ");
    int i = 0;
    char* params[3];
    while (tokens != NULL && i < 2) 
    {
        params[i] = tokens;
        tokens = strtok(NULL, " ");
        i++;
    }
    http_req* req = malloc(sizeof(http_req));
    hash_table* header_ht = malloc(sizeof(hash_table));
    header_ht->size = 0;
    req->headers = header_ht;
    char* method = params[0];
    parse_req_method(req, method);
    req->path = params[1];

    // parse request headers
    // skip requst line
    char* r_header = strstr(req_headers, "\r\n");
    r_header = strtok(r_header, "\r\n");
    while (r_header != NULL)
    {
        add_header(req, r_header);

        r_header = strtok(NULL, "\r\n");
    }

    free(req_headers);

    return req;
}


char* get_req_header(http_req* req, const char* name)
{
    if (req == NULL) 
    {
        return NULL;
    }

    return ht_get_value(req->headers, name);
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
