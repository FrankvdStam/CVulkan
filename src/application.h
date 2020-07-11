//
// Created by Frank on 06/07/2020.
//

#ifndef CVULKAN_APPLICATION_H
#define CVULKAN_APPLICATION_H

#include "common.h"
#include "utility/string_list.h"

typedef enum
{
    vulkan_debugging_enabled,
    vulkan_debugging_disabled,
} vulkan_debugging_mode_t;

typedef struct
{
    uint32_t graphics_family_index;
    uint32_t present_family_index;
}queue_family_indices_t;

typedef struct {
    VkSurfaceCapabilitiesKHR vk_surface_capabilities;
    uint32_t surface_formats_size;
    VkSurfaceFormatKHR* vk_surface_formats;
    uint32_t present_modes_size;
    VkPresentModeKHR* vk_surface_present_modes;
} swapchain_details_t;

typedef struct
{
    char* title;
    int window_with;
    int window_height;
    vulkan_debugging_mode_t vulkan_debugging_mode;

    GLFWwindow* glfw_window;

    //Vulkan
    VkInstance vk_instance;
    VkPhysicalDevice vk_physical_device;
    VkDevice vk_device;
    VkDebugUtilsMessengerEXT vk_debug_messenger;
    VkSurfaceFormatKHR vk_surface_format;
    VkPresentModeKHR vk_present_mode;
    VkExtent2D vk_extent;
    VkSwapchainKHR vk_swapchain;
    VkPipelineLayout vk_pipeline_layout;
    VkRenderPass vk_render_pass;

    uint32_t vk_image_size;
    VkImage* vk_images;
    VkImageView* vk_image_views;

    VkSurfaceKHR vk_surface;
    queue_family_indices_t queue_family_indices;
    swapchain_details_t swapchain_details;

    VkQueue vk_present_queue;
    VkQueue vk_graphics_queue;

    string_list_t* required_extension_names;
    string_list_t* required_layer_names;
} application_t;





application_t* application_init(int window_with, int window_height, char* title, vulkan_debugging_mode_t vulkan_debugging_mode);
void application_run(application_t* application);
//void application_cleanup(application_t* application);

#endif //CVULKAN_APPLICATION_H
