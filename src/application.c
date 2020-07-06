//
// Created by Frank on 06/07/2020.
//

#include "application.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
GLFWwindow* window;

VkInstance static_vk_instance;
//VkDebugUtilsMessengerEXT static_debug_messenger;

void run()
{
    init_vulkan();
    main_loop();
    cleanup();
}



void init_vulkan()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
    create_instance();
    //setup_debug_message_callback();
}

void main_loop()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void cleanup()
{
    glfwDestroyWindow(window);
    vkDestroyInstance(static_vk_instance, NULL);
    glfwTerminate();
}


void create_instance()
{
    VkApplicationInfo vk_app_info;
    vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_app_info.pApplicationName = "CVulkan";
    vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_app_info.pEngineName = "No Engine";
    vk_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo vk_instance_create_info;
    vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_instance_create_info.pApplicationInfo = &vk_app_info;

    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    vk_instance_create_info.enabledExtensionCount = glfw_extension_count;
    vk_instance_create_info.ppEnabledExtensionNames = glfw_extensions;

    const char* layer_names[] = { "VK_LAYER_KHRONOS_validation" };
    vk_instance_create_info.enabledLayerCount = 1;
    vk_instance_create_info.ppEnabledLayerNames = layer_names;

    if (vkCreateInstance(&vk_instance_create_info, NULL, &static_vk_instance) != VK_SUCCESS) {
        printf("failed to create instance!\n");
        exit(1);
    }
    else
    {
        printf("Successfully created instance!\n");
    }
}




////==========================================================================================================================================
////Debugging callbacks
//
////The actual callback
//static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
//        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//        VkDebugUtilsMessageTypeFlagsEXT messageType,
//        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//        void* pUserData
//    )
//{
//    printf("validation layer: %s\n", pCallbackData->pMessage);
//    return VK_FALSE;
//}
//
////creating an extension for the debug callback
//VkResult CreateDebugUtilsMessengerEXT(VkInstance vk_instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
//    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//    if (func != NULL) {
//        return func(vk_instance, pCreateInfo, pAllocator, pDebugMessenger);
//    } else {
//        return VK_ERROR_EXTENSION_NOT_PRESENT;
//    }
//}
//
////deleting an extension for the debug callback
//void DestroyDebugUtilsMessengerEXT(VkInstance vk_instance, VkDebugUtilsMessengerEXT vk_debugMessenger, const VkAllocationCallbacks* pAllocator) {
//    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//    if (func != NULL) {
//        func(vk_instance, vk_debugMessenger, pAllocator);
//    }
//}
//
////Setup the callback
//void setup_debug_message_callback()
//{
//    printf("as\n");
//    VkDebugUtilsMessengerCreateInfoEXT createInfo;
//    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//    createInfo.pfnUserCallback = debug_callback;
//    createInfo.pUserData = NULL; // Optional
//    printf("create ext\n");
//    VkResult tjomg = CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger);
//
//
//    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
//        printf("failed to set up debug messenger!\n");
//        exit(1);
//    }
//}

