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

    for(uint32_t i = 0; i < application->vk_image_size; i++)
    {
        vkDestroyImageView(application->vk_device, application->vk_image_views[i], NULL);
    }

    free(application->vk_images);
    vkDestroySwapchainKHR(application->vk_device, application->vk_swapchain, NULL);

    free(application->swapchain_details.vk_surface_present_modes);
    free(application->swapchain_details.vk_surface_formats);

    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        free_debug_utils_messenger_extension(application);
    }
    vkDestroySurfaceKHR(application->vk_instance, application->vk_surface, NULL);
    vkDestroyDevice(application->vk_device, NULL);
    vkDestroyInstance(application->vk_instance, NULL);

    string_list_free(application->required_extension_names);
    string_list_free(application->required_layer_names);

    glfwDestroyWindow(application->glfw_window);
    glfwTerminate();

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
    strcpy(application->title, title);
    application->vulkan_debugging_mode = vulkan_debugging_mode;

    //Initialize glfw and get a window
    application->glfw_window = glfw_init_get_window(application);

    //now that glfw is initialized, we can ask it about what extensions it needs
    application->required_extension_names = get_required_extensions(application);
    application->required_layer_names = get_required_layers(application);

    //Can now initialize vulkan
    application->vk_instance            = get_instance(application);
    application->vk_debug_messenger     = get_debug_callback(application);
    application->vk_surface             = get_vk_surface(application);
    application->vk_physical_device     = get_physical_device(application);
    application->queue_family_indices   = get_queue_family_indices(application);
    application->vk_device              = get_logical_device(application);
    application->vk_graphics_queue      = get_graphics_queue(application);
    application->vk_present_queue       = get_present_queue(application);
    application->swapchain_details      = get_swapchain_details(application);
    application->vk_surface_format      = get_surface_format(application);
    application->vk_present_mode        = get_present_mode(application);
    application->vk_extent              = get_swap_extent(application);
    application->vk_swapchain           = get_swapchain(application);
    uint32_t images_size = 0;
    application->vk_images              = get_swapchain_images(application, &images_size);
    application->vk_image_size = images_size;
    application->vk_image_views         = get_image_views(application);
    return application;
}


void application_run(application_t* application)
{
    while (!glfwWindowShouldClose(application->glfw_window)) {
        glfwPollEvents();
        glfwSetWindowShouldClose(application->glfw_window, 1);
    }

    application_cleanup(application);
}








