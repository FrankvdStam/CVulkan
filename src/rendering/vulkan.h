//
// Created by Frank on 10/07/2020.
//

#ifndef CVULKAN_VULKAN_H
#define CVULKAN_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../application.h"
#include "extensions.h"

VkSurfaceKHR get_vk_surface(const application_t* application);
queue_family_indices_t get_queue_family_indices(VkPhysicalDevice device);
void create_logical_device(application_t* application);
VkDebugUtilsMessengerEXT setup_debug_message_callback(application_t* application);
GLFWwindow* init_glfw_get_window(const application_t* application);
string_list_t* get_required_extensions(const application_t* application);
string_list_t* get_required_layers(const application_t* application);
VkInstance create_vulkan_instance(const application_t* application);
void pick_physical_device(application_t* application);

void DestroyDebugUtilsMessengerEXT(VkInstance vk_instance, VkDebugUtilsMessengerEXT vk_debugMessenger, const VkAllocationCallbacks* pAllocator);













#endif //CVULKAN_VULKAN_H
