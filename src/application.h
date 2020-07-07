//
// Created by Frank on 06/07/2020.
//

#ifndef CVULKAN_APPLICATION_H
#define CVULKAN_APPLICATION_H

#include "common.h"

#define VULKAN_DEBUG

typedef struct
{
    char* title;
    int window_with;
    int window_height;
    GLFWwindow* glfw_window;
    VkInstance vk_instance;
    VkDebugUtilsMessengerEXT debug_messenger;
} application_t;



application_t* application_init(int window_with, int window_height, char* title);
void application_run(application_t* application);
//void application_cleanup(application_t* application);

#endif //CVULKAN_APPLICATION_H
