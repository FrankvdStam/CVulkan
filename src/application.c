//
// Created by Frank on 06/07/2020.
//

#include "application.h"
#include "utility/string_list.h"
#include "extensions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


//========================================================================================================================================
//Private

//==========================================================================================================================================
//Debugging callbacks

//The actual callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
)
{
    printf("validation layer: %s\n", pCallbackData->pMessage);
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
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debug_callback;
        createInfo.pUserData = NULL; // Optional
        createInfo.flags = 0;

        if (CreateDebugUtilsMessengerEXT(application->vk_instance, &createInfo, NULL, &application->debug_messenger) != VK_SUCCESS) {
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

string_list_t get_required_extensions(application_t* application)
{
    //Create a stringlist to keep track of all the names of extensions we want to enable.
    string_list_t list = string_list_init(&list, 4);

    //fetch all the extensions that glfw needs and add all of the existing extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    for(size_t i = 0; i < glfw_extension_count; i++)
    {
        if(extension_exists(glfw_extensions[i]))
        {
            string_list_add(&list, (char*)glfw_extensions[i]);
        }
        else
        {
            printf("glfw requests extension %s, which is not available on the current system. Proceeding without it...", glfw_extensions[i]);
        }
    }

    //Add debug utils if vulkan debugging is enabled
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        string_list_add(&list, "VK_EXT_debug_utils");
    }
    return list;
}

void create_vulkan_instance(application_t* application)
{
    //Get the required extensions based on glfw and debugging mode
    string_list_t required_extensions = get_required_extensions(application);

    //get_extensions(application);
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



    vk_instance_create_info.enabledExtensionCount = required_extensions.current_index;
    vk_instance_create_info.ppEnabledExtensionNames = (const char **) required_extensions.data;


    if(vulkan_debugging_enabled)
    {
        const char* layer_names[] = { "VK_LAYER_KHRONOS_validation" };
        vk_instance_create_info.enabledLayerCount = 1;
        vk_instance_create_info.ppEnabledLayerNames = layer_names;
    }
    else
    {
        vk_instance_create_info.ppEnabledLayerNames = NULL;
        vk_instance_create_info.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&vk_instance_create_info, NULL, &application->vk_instance) != VK_SUCCESS) {
        printf("failed to create instance!\n");
        exit(1);
    }
    else
    {
        printf("Successfully created instance!\n");
    }

    string_list_free(&required_extensions);
}




void init_vulkan(application_t* application)
{
    create_vulkan_instance(application);
    setup_debug_message_callback(application);
}


//========================================================================================================================================
//Cleanup
void application_cleanup(application_t* application)
{
    DestroyDebugUtilsMessengerEXT(application->vk_instance, application->debug_messenger, NULL);
    vkDestroyInstance(application->vk_instance, NULL);

    glfwDestroyWindow(application->glfw_window);
    glfwTerminate();

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








