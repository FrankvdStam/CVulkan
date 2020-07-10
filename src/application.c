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
    application_t* application = (application_t*)malloc(sizeof(application_t));
    application->window_with = window_with;
    application->window_height = window_height;
    application->title = (char*)malloc(sizeof(char) * strlen(title));
    application->vulkan_debugging_mode = vulkan_debugging_mode;
    strcpy(application->title, title);

    //Initialize glfw and get a window
    application->glfw_window = init_glfw_get_window(application);

    //now that glfw is initialized, we can ask it about what extensions it needs
    application->required_extension_names = get_required_extensions(application);
    application->required_layer_names = get_required_layers(application);

    //Can now initialize vulkan
    application->vk_instance = create_vulkan_instance(application);
    application->vk_debug_messenger = setup_debug_message_callback(application);
    application->vk_surface = get_vk_surface(application);
    application->vk_physical_device = pick_physical_device(application);
    application->queue_family_indices = get_queue_family_indices(application);
    application->vk_device = create_logical_device(application);
    application->vk_graphics_queue = get_graphics_queue(application);

    return application;
}


void application_run(application_t* application)
{
    while (!glfwWindowShouldClose(application->glfw_window)) {
        glfwPollEvents();
        glfwWindowShouldClose(application->glfw_window);
    }
    application_cleanup(application);
}








