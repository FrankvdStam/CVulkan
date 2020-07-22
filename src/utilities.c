//
// Created by Frank on 04/05/2020.
//

#include "utilities.h"
#include "common.h"

void read_file(const char* file_path, unsigned char** buffer, unsigned long* file_size)
{
    FILE* file;
    //char* buffer;
    //unsigned long file_size;

    //Open file
    file = fopen(file_path, "rb");
    if (!file)
    {
        fprintf(stderr, "Unable to open file %s", file_path);
        return;
    }

    //Get file length
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    //Allocate memory
    *buffer = (unsigned char *)malloc(*file_size + 1);
    if (!*buffer)
    {
        fprintf(stderr, "Memory error!");
        fclose(file);
        return;
    }

    //Read file contents into buffer
    fread(*buffer, *file_size, 1, file);
    fclose(file);
}

void mat4_zero_initialize(mat4* mat)
{
    for(size_t y = 0; y < 4u; y++)
    {
        for(size_t x = 0; x < 4u; x++)
        {
            *mat[y][x] = 1.0f;
        }
    }
}