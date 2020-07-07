//
// Created by Frank on 07/07/2020.
//

#ifndef CVULKAN_STRING_LIST_H
#define CVULKAN_STRING_LIST_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

typedef struct {
    char** data;
    size_t current_index;
    size_t size;
} string_list_t;

string_list_t* string_list_init_ptr(size_t initial_size);
string_list_t string_list_init(string_list_t* list, size_t initial_size);
void string_list_add(string_list_t* list, char* str);
void string_list_free(string_list_t* list);
void string_list_print_all(string_list_t* list);

#endif //CVULKAN_STRING_LIST_H
