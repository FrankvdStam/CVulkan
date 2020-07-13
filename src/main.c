#include "common.h"
#include "application.h"
#include "utilities.h"

/*
"3
2
35
7"
*/



//50471687	wrong
//119734787,	right





/*


void doit() {
    //Read the shader file into a char buffer
    unsigned char *file_buffer;
    unsigned long file_size;
    read_file("C:\\projects\\CVulkan\\shaders\\vert.spv", &file_buffer, &file_size);

    uint32_t code_size = file_size / sizeof(uint32_t);
    uint32_t *shader_code = (uint32_t *) malloc(code_size * sizeof(uint32_t));

    //for(int i =0; i < 4; i++)
    //{
    //    printf("%i\n", file_buffer[i]);
    //}

    for (size_t i = 0; i < code_size; i++) {
        //Calculate the proper index into the char buffer
        size_t start_index = i * sizeof(uint32_t);

        //Reinterpret the data in the char buffer as a uint32_t
        uint32_t number = (uint32_t) file_buffer[start_index + 0] |
                          (uint32_t) file_buffer[start_index + 1] << 8u |
                          (uint32_t) file_buffer[start_index + 2] << 16u |
                          (uint32_t) file_buffer[start_index + 3] << 24u;

        //uint32_t a = file_buffer[start_index + 0];
        //uint32_t b = ((uint32_t)file_buffer[start_index + 1]) << 8u;
        //uint32_t c = ((uint32_t)file_buffer[start_index + 2]) << 16u;
        //uint32_t d = ((uint32_t)file_buffer[start_index + 3]) << 24u;
//
        //uint32_t alt = a | b | c | d;
//
        //uint32_t alt2 = strtol(&file_buffer[i], NULL, 10);
//
        //if(number != alt2)
        //{
        //    printf("Numbers not equal! %zu: %u != %u\n", i, number, alt2);
        //}
//

        //Add the number to the shader code
        shader_code[i] = number;
    }



    if(vertex_shader_source_size != code_size)
    {
        printf("Shader source size doesn't match");
        exit(1);
    }

    for(size_t i = 0; i < vertex_shader_source_size; i++)
    {
        if(vertex_shader_source[i] != shader_code[i])
        {
            char str[4];
            uint32_t index = i*4;
            str[0] = file_buffer[index];
            str[1] = file_buffer[index+1];
            str[2] = file_buffer[index+2];
            str[3] = file_buffer[index+3];
            printf("%zu %u != %u. Original chars: %i %i %i %i\n", i, vertex_shader_source[i], shader_code[i], str[0], str[1], str[2], str[3]);
        }
    }

    //printf("File buffer contents:\n");
    //for(size_t i = 0; i < file_size; i++)
    //{
    //    printf("%u\n", (unsigned char)file_buffer[i]);
    //}
    //printf("\n");

    free(file_buffer);
}




*/

int main(int argc, char *argv[])
{
    //doit();
    //exit(0);


    //char string[4];
    //string[0] = 3;
    //string[1] = 2;
    //string[2] = 35;
    //string[3] = 7;
//
    //size_t start_index = 0;
    //uint32_t number =   string[0] << 24 |
    //                    string[1] << 16 |
    //                    string[2] << 8  |
    //                    string[3];
//
//
    //uint32_t number2 =  string[0]           |
    //                    string[1] << 8      |
    //                    string[2] << 16     |
    //                    string[3] << 24;
//
    //uint32_t a_toi = strtol(string, &string[3], 4);
//
    //printf("shifting: %u\n", number);
    //printf("shifting reverse: %u\n", number2);
    //printf("atoi: %u", a_toi);
    //exit(1);



/*
    string_list_t list;
    string_list_init(&list, 4);
    string_list_add(&list, "Hosterd!");
    string_list_add(&list, "Mosterd!");
    string_list_add(&list, "boster!");
    string_list_add(&list, "wosterd!");
    string_list_add(&list, "gosterd!");


    for(size_t i = 0; i < list.next_index; i++)
    {
        printf("%s\n", list.data[i]);
    }

    printf("List size: %u\n", list.size);
*/

    //int val = heaptostack();
    //printf("after ret: %i\n", val);

    application_t* app = application_init(800, 600, "Vulkan", vulkan_debugging_enabled);
    //application_t* app = application_init(800, 600, "Vulkan", vulkan_debugging_disabled);
    application_run(app);
    return 0;
}