//
// Created by Frank on 06/07/2020.
//

#ifndef CVULKAN_APPLICATION_H
#define CVULKAN_APPLICATION_H

#include "common.h"
#include "utility/string_list.h"
#include <cglm/cglm.h>
#include <stdalign.h>
#include "utilities.h"

const int MAX_FRAMES_IN_FLIGHT;

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

typedef struct  {
    vec2 pos;
    vec3 color;
} vertex_t;

typedef struct
{
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
} uniform_buffer_t;

uniform_buffer_t uniform_buffer_create();


typedef struct
{
    char* title;
    int window_width;
    int window_height;
    vulkan_debugging_mode_t vulkan_debugging_mode;
    size_t current_frame;
    time_t start_time;

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
    VkDescriptorSetLayout vk_descriptor_set_layout;
    VkPipeline vk_graphics_pipeline;
    VkCommandPool vk_command_pool;
    VkDescriptorPool vk_descriptor_pool;
    VkDescriptorSet* vk_descriptor_sets;
    VkPhysicalDeviceMemoryProperties vk_memory_properties;

    VkBuffer vk_vertex_buffer;
    VkDeviceMemory vk_vertex_buffer_memory;
    VkBuffer vk_index_buffer;
    VkDeviceMemory vk_index_buffer_memory;
    size_t vk_uniform_buffers_size;
    VkBuffer* vk_uniform_buffers;
    VkDeviceMemory* vk_uniform_buffers_memory;


    VkSemaphore* vk_image_available_semaphore;
    VkSemaphore* vk_render_finished_semaphore;
    VkFence* vk_fences;
    VkFence* vk_image_in_flight_fences;

    uint32_t swapchain_images_size;
    VkImage* vk_images;
    VkImageView* vk_image_views;
    VkFramebuffer* vk_frame_buffers;
    VkCommandBuffer* vk_command_buffers;

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
void application_rebuild_swapchain(application_t* application);
//void application_cleanup(application_t* application);

#endif //CVULKAN_APPLICATION_H
