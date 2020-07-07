#include "common.h"
#include "utilities.h"
#include <vulkan/vulkan.h>

#include "application.h"
#include "utility/string_list.h"

int main(int argc, char *argv[]) {
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


    application_t* app = application_init(800, 600, "Vulkan", vulkan_debugging_enabled);
    //application_t* app = application_init(800, 600, "Vulkan", vulkan_debugging_disabled);
    application_run(app);
    return 0;
}