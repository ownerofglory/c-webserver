#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char* read_file(char* name) 
{
    FILE* file = fopen(name, "r");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buf = malloc(file_size + 1);
    if (buf == NULL) 
    {
        printf("Error when allocating file buffer");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buf, 1, file_size, file);
    if (bytes_read < file_size) 
    {
        printf("Error when reading file");
        fclose(file);
        free(buf);
        return NULL;
    }
    
    buf[file_size] = '\0';

    fclose (file);

    return buf;
}