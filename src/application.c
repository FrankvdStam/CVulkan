//
// Created by Frank on 06/07/2020.
//

#include "application.h"

#include "extensions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


//========================================================================================================================================
//Private

//========================================================================================================================================
//queue families

queue_family_indices_t get_queue_family_indices(VkPhysicalDevice device) {
    queue_family_indices_t indices;

    indices.graphics_family_index = 0;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    VkQueueFamilyProperties* queue_family_properties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

    for(size_t i = 0; i < queue_family_count; i++)
    {
        if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.has_graphics_family_index = true;
            indices.graphics_family_index = i;
        }
    }

    free(queue_family_properties);
    return indices;
}

//==========================================================================================================================================
//Logical device
void create_logical_device(application_t* application)
{
    queue_family_indices_t indices = get_queue_family_indices(application->vk_physical_device);

    VkDeviceQueueCreateInfo queue_create_info;
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = indices.graphics_family_index;
    queue_create_info.queueCount = 1;
    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features;

    VkDeviceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = 0;

    //if (application->vulkan_debugging_mode == vulkan_debugging_enabled)
    //{
    //    const char* layer_names[] = { "VK_LAYER_KHRONOS_validation" };
    //    createInfo.enabledLayerCount = 1;
    //    createInfo.ppEnabledLayerNames = layer_names;
    //}
    //else
    //{
    //    createInfo.enabledLayerCount = 0;
    //}

    if (vkCreateDevice(application->vk_physical_device, &create_info, NULL, &application->vk_device) != VK_SUCCESS)
    {
        printf("failed to create logical device!\n");
        exit(1);
    }
}


//==========================================================================================================================================
//Debugging callbacks

//The actual callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
        void* p_user_data
)
{
    printf("validation layer: %s\n", p_callback_data->pMessage);
    return VK_FALSE;
}

//creating an extension for the debug callback
VkResult CreateDebugUtilsMessengerEXT(VkInstance vk_instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vk_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(vk_instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//deleting an extension for the debug callback
void DestroyDebugUtilsMessengerEXT(VkInstance vk_instance, VkDebugUtilsMessengerEXT vk_debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(vk_instance, vk_debugMessenger, pAllocator);
    }
}

//Setup the callback
void setup_debug_message_callback(application_t* application)
{
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        VkDebugUtilsMessengerCreateInfoEXT create_info;
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debug_callback;
        create_info.pUserData = NULL; // Optional
        create_info.flags = 0;

        if (CreateDebugUtilsMessengerEXT(application->vk_instance, &create_info, NULL, &application->vk_debug_messenger) != VK_SUCCESS) {
            printf("failed to set up debug messenger!\n");
            exit(1);
        } else{
            printf("Sucessfully set up debug messenger!\n");
        }
    }
}

//===========================================================================================================================================
//Initialization

void init_glfw(application_t* application)
{
    if(glfwInit() == GLFW_FALSE)
    {
        printf("Failed to initialize glfw.\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    application->glfw_window = glfwCreateWindow(application->window_with, application->window_height, "Vulkan", NULL, NULL);
    if(application->glfw_window == NULL)
    {
        printf("Failed to create glfw window.\n");
        exit(1);
    }
}

string_list_t* get_required_extensions(application_t* application)
{
    //Create a stringlist to keep track of all the names of extensions we want to enable.
    string_list_t* list = (string_list_t*)malloc(sizeof(string_list_t));
    string_list_init(list, 4);

    //fetch all the extensions that glfw needs and add all of the existing extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    for(size_t i = 0; i < glfw_extension_count; i++)
    {
        if(extension_exists(glfw_extensions[i]))
        {
            string_list_add(list, (char*)glfw_extensions[i]);
        }
        else
        {
            printf("glfw requests extension %s, which is not available on the current system. Proceeding without it...", glfw_extensions[i]);
        }
    }

    //Add debug utils if vulkan debugging is enabled
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        string_list_add(list, "VK_EXT_debug_utils");
    }
    return list;
}

string_list_t* get_required_layers(application_t* application)
{
    string_list_t* list = (string_list_t*)malloc(sizeof(string_list_t));
    string_list_init(list, 1);
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        string_list_add(list, "VK_LAYER_KHRONOS_validation");
    }
    return list;
}

void create_vulkan_instance(application_t* application)
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

    application->required_extension_names = get_required_extensions(application);
    application->required_layer_names = get_required_layers(application);

    vk_instance_create_info.enabledExtensionCount = application->required_extension_names->current_index;
    vk_instance_create_info.ppEnabledExtensionNames = (const char**) application->required_extension_names->data;


    vk_instance_create_info.enabledLayerCount = application->required_layer_names->current_index;
    vk_instance_create_info.ppEnabledLayerNames = (const char**)application->required_layer_names->data;

    if (vkCreateInstance(&vk_instance_create_info, NULL, &application->vk_instance) != VK_SUCCESS)
    {
        printf("failed to create instance!\n");
        exit(1);
    }
    else
    {
        printf("Successfully created instance!\n");
    }


}

void pick_physical_device(application_t* application)
{
    //Get the devices
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(application->vk_instance, &device_count, NULL);
    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * device_count);
    vkEnumeratePhysicalDevices(application->vk_instance, &device_count, devices);

    if(device_count == 0 || devices[0] == VK_NULL_HANDLE)
    {
        printf("No devices with vulkan support found.");
        exit(1);
    }

    application->vk_physical_device = devices[0];
    free(devices);

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(application->vk_physical_device, &deviceProperties);

    printf("Picked device %s\n", deviceProperties.deviceName);
}


void init_vulkan(application_t* application)
{
    create_vulkan_instance(application);
    setup_debug_message_callback(application);
    pick_physical_device(application);
    //create_logical_device(application);
}


//========================================================================================================================================
//Cleanup
void application_cleanup(application_t* application)
{
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        DestroyDebugUtilsMessengerEXT(application->vk_instance, application->vk_debug_messenger, NULL);
    }
    //vkDestroyDevice(application->vk_device, NULL);
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








