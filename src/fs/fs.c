#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fs.h"

int file_size(const char* name)
{
    FILE* file = fopen(name, "r");
    if (file == NULL) {
        printf("Error when opening file\n");
        return ERR_FILE_NOT_FOUND;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    fclose(file);

    return file_size;
}

int read_file(const char* name, char* buf, size_t buf_size) 
{
    FILE* file = fopen(name, "r");
    if (file == NULL) {
        printf("Error when opening file\n");
        return ERR_FILE_NOT_FOUND;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (buf_size < file_size + 1)
    {
        printf("Provided buffer is too small\n");
        fclose(file);
        return ERR_BUF_TOO_SMALL;
    }

    size_t bytes_read = fread(buf, 1, file_size, file);
    if (bytes_read < file_size) 
    {
        printf("Error when reading file");
        fclose(file);
        free(buf);
        return ERR_READ_FAIL;
    }
    
    buf[file_size] = '\0';

    fclose (file);

    return SUCCESS;
}