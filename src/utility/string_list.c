//
// Created by Frank on 07/07/2020.
//

#include "string_list.h"


string_list_t* string_list_init_ptr(size_t initial_size) {
    string_list_t* list = (string_list_t*)malloc(sizeof(string_list_t));
    list->data = malloc(initial_size * sizeof(char*));
    list->current_index = 0;
    list->size = initial_size;
    return list;
}

void string_list_init(string_list_t* list, size_t initial_size)
{
    list->data = malloc(initial_size * sizeof(char*));
    list->current_index = 0;
    list->size = initial_size;
}

void string_list_add(string_list_t* list, char* str)
{
    if (list->current_index == list->size) {
        list->size *= 2;
        list->data = realloc(list->data, list->size * sizeof(char*));
    }
    char* addstr = (char*)malloc(sizeof(char) * (strlen(str)+1));
    strcpy(addstr, str);
    list->data[list->current_index++] = addstr;
}

void string_list_free_ptr(string_list_t* list) {
    free(list->data);
    free(list);
}

void string_list_free(string_list_t* list) {
    for(size_t i = 0; i < list->current_index; i++)
    {
        free(list->data[i]);
    }
    free(list->data);
}

void string_list_print_all(string_list_t* list)
{
    for(size_t i = 0; i < list->current_index; i++)
    {
        printf("%s\n", list->data[i]);
    }
}