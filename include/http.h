#ifndef HTTP_H
#define HTTP_H

typedef enum 
{
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_HEAD
} http_req_method;

typedef enum 
{
    HTTP_RESP_OK = 200,
    HTTP_RESP_BAD_REQUEST = 400,
    HTTP_RESP_NOT_FOUND = 404,
    HTTP_RESP_INTERNAL_SERVER_ERROR = 500
} http_resp_code;

typedef struct
{
    char* name;
    char* value;
} http_header;


typedef struct 
{
    http_req_method method;
    char* path;
    http_header* headers;
} http_req;

http_req* parse_req(int* sock_fd);

int write_resp(int* sock_fd, int status, char* headers, char* body);

#endif