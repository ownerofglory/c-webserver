#ifndef FS_H
#define FS_H

#define SUCCESS 0
#define ERR_FILE_NOT_FOUND -1
#define ERR_BUF_TOO_SMALL -2
#define ERR_READ_FAIL -3

// char* read_file(char* name);

int file_size(const char* name);

int read_file(const char* name, char* buf, size_t buf_size);

#endif