//
// Created by Frank on 06/07/2020.
//

#include "application.h"

#include "rendering/extensions.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "rendering/vulkan.h"

//========================================================================================================================================
//Private

void init_vulkan(application_t* application)
{
    create_vulkan_instance(application);
    application->vk_debug_messenger = setup_debug_message_callback(application);
    application->vk_surface = get_vk_surface(application);
    pick_physical_device(application);
    create_logical_device(application);
}


//========================================================================================================================================
//Cleanup
void application_cleanup(application_t* application)
{
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        DestroyDebugUtilsMessengerEXT(application->vk_instance, application->vk_debug_messenger, NULL);
    }
    vkDestroySurfaceKHR(application->vk_instance, application->vk_surface, NULL);
    vkDestroyDevice(application->vk_device, NULL);
    vkDestroyInstance(application->vk_instance, NULL);

    glfwDestroyWindow(application->glfw_window);
    glfwTerminate();

    string_list_free(application->required_extension_names);
    string_list_free(application->required_layer_names);
    free(application->title);
    free(application);
}

//========================================================================================================================================
//public

application_t* application_init(int window_with, int window_height, char* title, vulkan_debugging_mode_t vulkan_debugging_mode)
{
    application_t* app = (application_t*)malloc(sizeof(application_t));
    app->window_with = window_with;
    app->window_height = window_height;
    app->title = (char*)malloc(sizeof(char) * strlen(title));
    app->vulkan_debugging_mode = vulkan_debugging_mode;
    strcpy(app->title, title);

    init_glfw(app);
    init_vulkan(app);
    return app;
}


void application_run(application_t* application)
{
    while (!glfwWindowShouldClose(application->glfw_window)) {
        glfwPollEvents();
    }
    application_cleanup(application);
}








