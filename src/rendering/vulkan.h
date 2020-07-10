//
// Created by Frank on 10/07/2020.
//

#ifndef CVULKAN_VULKAN_H
#define CVULKAN_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../application.h"
#include "extensions.h"

queue_family_indices_t      get_queue_family_indices(               const application_t* application);
string_list_t*              get_required_extensions(                const application_t* application);
string_list_t*              get_required_layers(                    const application_t* application);

GLFWwindow*                 glfw_init_get_window(                   const application_t* application);
VkSurfaceKHR                get_vk_surface(                         const application_t* application);

VkDebugUtilsMessengerEXT    setup_debug_message_callback(           const application_t* application);

VkInstance                  create_vulkan_instance(                 const application_t* application);
VkPhysicalDevice            pick_physical_device(                   const application_t* application);

VkDevice                    create_logical_device(                  const application_t* application);
VkQueue                     get_graphics_queue(                     const application_t* application);
VkQueue                     get_present_queue(                      const application_t* application);

swapchain_details_t         get_swapchain_details(                  const application_t* application);
VkSurfaceFormatKHR          get_surface_format(                     const application_t* application);
VkPresentModeKHR            get_present_mode(                       const application_t* application);
VkExtent2D                  get_swap_extent(                        const application_t* application);
VkSwapchainKHR              get_swapchain(                          const application_t* application);

void                        free_debug_utils_messenger_extension(   const application_t* application);


#endif //CVULKAN_VULKAN_H
