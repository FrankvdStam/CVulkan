//
// Created by Frank on 10/07/2020.
//

#include "vulkan.h"
#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//========================================================================================================================================
//surface


VkPhysicalDeviceMemoryProperties get_memory_properties(const application_t* application)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(application->vk_physical_device, &memory_properties);
    return memory_properties;
}

uint32_t find_memory_index(const application_t* application, uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < application->vk_memory_properties.memoryTypeCount; i++) {
        if ((type_filter & (1u << i)) && (application->vk_memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    printf("Failed to get memory type.");
    exit(1);
}


VkSurfaceKHR get_vk_surface(const application_t* application)
{
    VkSurfaceKHR vk_surface;
    if (glfwCreateWindowSurface(application->vk_instance, application->glfw_window, NULL, &vk_surface) != VK_SUCCESS)
    {
        printf("failed to create window surface\n");
        exit(1);
    }

    printf("Successfully created window surface\n");
    return vk_surface;
}

//========================================================================================================================================
//queue families

queue_family_indices_t get_queue_family_indices(const application_t* application) {
    bool has_graphics_family_index = false;
    bool has_present_family_index = false;

    queue_family_indices_t indices;

    indices.graphics_family_index = 0;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(application->vk_physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties* queue_family_properties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(application->vk_physical_device, &queue_family_count, queue_family_properties);

    for(size_t i = 0; i < queue_family_count; i++)
    {
        if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            has_graphics_family_index = true;
            indices.graphics_family_index = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(application->vk_physical_device, i, application->vk_surface, &presentSupport);
        if(presentSupport)
        {
            indices.present_family_index = i;
            has_present_family_index = true;
        }
    }

    if(!has_graphics_family_index)
    {
        printf("No graphics queue family index found.");
        exit(1);
    }

    if(!has_present_family_index)
    {
        printf("No present queue family index found.");
        exit(1);
    }

    free(queue_family_properties);
    return indices;
}

//==========================================================================================================================================
//Logical device


#define TOTAL_QUEUE_INDICES 2

VkDevice get_logical_device(const application_t* application)
{
    VkDevice vk_device;

    //setup a queue for each index we have
    uint32_t indices[TOTAL_QUEUE_INDICES];
    VkDeviceQueueCreateInfo queue_create_infos[TOTAL_QUEUE_INDICES];

    indices[0] = application->queue_family_indices.graphics_family_index;
    indices[1] = application->queue_family_indices.present_family_index;


    for(uint32_t i = 0; i < TOTAL_QUEUE_INDICES; i++)
    {
        VkDeviceQueueCreateInfo queue_create_info;
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = indices[i];
        queue_create_info.queueCount = 1;
        queue_create_info.pNext = NULL;
        queue_create_info.flags = 0;
        float queuePriority = 1.0f;
        queue_create_info.pQueuePriorities = &queuePriority;
        queue_create_infos[i] = queue_create_info;
    }

    VkPhysicalDeviceFeatures device_features;
    device_features.robustBufferAccess = VK_FALSE;
    device_features.fullDrawIndexUint32 = VK_FALSE;
    device_features.imageCubeArray = VK_FALSE;
    device_features.independentBlend = VK_FALSE;
    device_features.geometryShader = VK_FALSE;
    device_features.sampleRateShading = VK_FALSE;
    device_features.logicOp = VK_FALSE;
    device_features.drawIndirectFirstInstance = VK_FALSE;
    device_features.depthBiasClamp = VK_FALSE;
    device_features.depthBounds = VK_FALSE;
    device_features.wideLines = VK_FALSE;
    device_features.largePoints = VK_FALSE;
    device_features.alphaToOne = VK_FALSE;
    device_features.multiViewport = VK_FALSE;
    device_features.samplerAnisotropy = VK_FALSE;
    device_features.textureCompressionETC2 = VK_FALSE;
    device_features.textureCompressionASTC_LDR = VK_FALSE;
    device_features.textureCompressionBC = VK_FALSE;
    device_features.occlusionQueryPrecise = VK_FALSE;
    device_features.pipelineStatisticsQuery = VK_FALSE;
    device_features.vertexPipelineStoresAndAtomics = VK_FALSE;
    device_features.fragmentStoresAndAtomics = VK_FALSE;
    device_features.shaderTessellationAndGeometryPointSize = VK_FALSE;
    device_features.shaderImageGatherExtended = VK_FALSE;
    device_features.shaderStorageImageExtendedFormats = VK_FALSE;
    device_features.shaderStorageImageMultisample = VK_FALSE;
    device_features.shaderStorageImageReadWithoutFormat = VK_FALSE;
    device_features.shaderStorageImageWriteWithoutFormat = VK_FALSE;
    device_features.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
    device_features.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
    device_features.shaderClipDistance = VK_FALSE;
    device_features.shaderCullDistance = VK_FALSE;
    device_features.shaderFloat64 = VK_FALSE;
    device_features.shaderInt64 = VK_FALSE;
    device_features.shaderInt16 = VK_FALSE;
    device_features.shaderResourceResidency = VK_FALSE;
    device_features.shaderResourceMinLod = VK_FALSE;
    device_features.sparseBinding = VK_FALSE;
    device_features.sparseResidencyBuffer = VK_FALSE;
    device_features.sparseResidencyImage2D = VK_FALSE;
    device_features.inheritedQueries = VK_FALSE;
    device_features.dualSrcBlend = VK_FALSE;
    device_features.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
    device_features.sparseResidency2Samples = VK_FALSE;
    device_features.sparseResidency8Samples = VK_FALSE;
    device_features.sparseResidencyAliased = VK_FALSE;
    device_features.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
    device_features.depthClamp = VK_FALSE;
    device_features.sparseResidency16Samples = VK_FALSE;

    VkDeviceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = TOTAL_QUEUE_INDICES;
    create_info.pQueueCreateInfos = queue_create_infos;
    create_info.pEnabledFeatures = &device_features;
    create_info.pNext = NULL;

    const char* device_extensions[] = { "VK_KHR_swapchain" };
    create_info.ppEnabledExtensionNames = device_extensions;
    create_info.enabledExtensionCount = 1;

    create_info.enabledLayerCount = application->required_layer_names->current_index;
    create_info.ppEnabledLayerNames = (const char**)application->required_layer_names->data;
    create_info.flags = 0;

    if (vkCreateDevice(application->vk_physical_device, &create_info, NULL, &vk_device) != VK_SUCCESS)
    {
        printf("failed to create logical device\n");
        exit(1);
    }
    else
    {
        printf("Successfully created logical device\n");
        return vk_device;
    }
}

VkQueue get_graphics_queue(const application_t* application)
{
    VkQueue vk_graphics_queue;
    vkGetDeviceQueue(application->vk_device, application->queue_family_indices.graphics_family_index, 0, &vk_graphics_queue);
    if(vk_graphics_queue == NULL)
    {
        printf("failed to create graphics queue\n");
        exit(1);
    }
    printf("Setup graphics queue\n");
    return vk_graphics_queue;
}


VkQueue get_present_queue(const application_t* application)
{
    VkQueue vk_present_queue;
    vkGetDeviceQueue(application->vk_device, application->queue_family_indices.present_family_index, 0, &vk_present_queue);
    if(vk_present_queue == NULL)
    {
        printf("failed to create present queue\n");
        exit(1);
    }
    printf("Setup present queue\n");
    return vk_present_queue;
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
    exit(1);
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

void free_debug_utils_messenger_extension(const application_t* application)
{
    DestroyDebugUtilsMessengerEXT(application->vk_instance, application->vk_debug_messenger, NULL);
}




//Setup the callback
VkDebugUtilsMessengerEXT get_debug_callback(const application_t* application)
{
    VkDebugUtilsMessengerEXT vk_debug_messenger;
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        VkDebugUtilsMessengerCreateInfoEXT create_info;
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debug_callback;
        create_info.pUserData = NULL; // Optional
        create_info.flags = 0;

        if (CreateDebugUtilsMessengerEXT(application->vk_instance, &create_info, NULL, &vk_debug_messenger) != VK_SUCCESS) {
            printf("failed to set up debug messenger\n");
            exit(1);
        } else {
            printf("Sucessfully set up debug messenger\n");
            return vk_debug_messenger;
        }
    }
    return NULL;
}

//===========================================================================================================================================
//Initialization

GLFWwindow* glfw_init_get_window(const application_t* application)
{
    GLFWwindow* glfw_window;
    if(glfwInit() == GLFW_FALSE)
    {
        printf("Failed to initialize glfw.\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfw_window = glfwCreateWindow(application->window_width, application->window_height, application->title, NULL, NULL);

    if(glfw_window == NULL)
    {
        printf("Failed to create glfw window.\n");
        exit(1);
    }

    return glfw_window;
}

string_list_t* get_required_extensions(const application_t* application)
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

string_list_t* get_required_layers(const application_t* application)
{
    string_list_t* list = (string_list_t*)malloc(sizeof(string_list_t));
    string_list_init(list, 1);
    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        string_list_add(list, "VK_LAYER_KHRONOS_validation");
    }
    return list;
}

VkInstance get_instance(const application_t* application)
{
    VkInstance vk_instance;

    VkApplicationInfo vk_app_info;
    vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_app_info.pApplicationName = "CVulkan";
    vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_app_info.pEngineName = "No Engine";
    vk_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_app_info.apiVersion = VK_API_VERSION_1_0;
    vk_app_info.pNext = NULL;

    VkInstanceCreateInfo vk_instance_create_info;
    vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_instance_create_info.pApplicationInfo = &vk_app_info;
    vk_instance_create_info.pNext = NULL;
    vk_instance_create_info.flags = 0;
    vk_instance_create_info.enabledExtensionCount = application->required_extension_names->current_index;
    vk_instance_create_info.ppEnabledExtensionNames = (const char**) application->required_extension_names->data;
    vk_instance_create_info.enabledLayerCount = application->required_layer_names->current_index;
    vk_instance_create_info.ppEnabledLayerNames = (const char**)application->required_layer_names->data;

    if (vkCreateInstance(&vk_instance_create_info, NULL, &vk_instance) != VK_SUCCESS)
    {
        printf("failed to create instance\n");
        exit(1);
    }
    printf("Successfully created instance\n");
    return vk_instance;
}

VkPhysicalDevice get_physical_device(const application_t* application)
{
    VkPhysicalDevice vk_physical_device;

    //Get the devices
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(application->vk_instance, &device_count, NULL);

    //Someone could have more than 10 devices. This is a potential memory issue.
    VkPhysicalDevice devices[10];
    if(device_count > 10)
    {
        printf("There are more physical devices than are allocated for.");
        exit(1);
    }

    vkEnumeratePhysicalDevices(application->vk_instance, &device_count, devices);

    if(device_count == 0 || devices[0] == VK_NULL_HANDLE)
    {
        printf("No devices with vulkan support found.");
        exit(1);
    }

    vk_physical_device = devices[0];

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(vk_physical_device, &deviceProperties);

    printf("Picked device %s\n", deviceProperties.deviceName);


    //printf("extensions:\n");
//
    //uint32_t extensions_count = 0;
    //vkEnumerateDeviceExtensionProperties(vk_physical_device, NULL, &extensions_count, NULL);
    //VkExtensionProperties* props = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensions_count);
    //vkEnumerateDeviceExtensionProperties(vk_physical_device, NULL, &extensions_count, props);
//
    //for(uint32_t i = 0; i < extensions_count; i++)
    //{
    //    printf("%s\n", props[i].extensionName);
    //}

    return vk_physical_device;
}


//===========================================================================================================================================
//swapchain
swapchain_details_t get_swapchain_details(const application_t* application)
{
    swapchain_details_t swapchain_details;

    //get the capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(application->vk_physical_device, application->vk_surface, &swapchain_details.vk_surface_capabilities);

    //get the formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(application->vk_physical_device, application->vk_surface, &swapchain_details.surface_formats_size, NULL);
    swapchain_details.vk_surface_formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * swapchain_details.surface_formats_size);
    vkGetPhysicalDeviceSurfaceFormatsKHR(application->vk_physical_device, application->vk_surface, &swapchain_details.surface_formats_size, swapchain_details.vk_surface_formats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(application->vk_physical_device, application->vk_surface, &swapchain_details.present_modes_size, NULL);
    swapchain_details.vk_surface_present_modes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * swapchain_details.present_modes_size);
    vkGetPhysicalDeviceSurfacePresentModesKHR(application->vk_physical_device, application->vk_surface, &swapchain_details.present_modes_size, swapchain_details.vk_surface_present_modes);

    return swapchain_details;
}

VkSurfaceFormatKHR get_surface_format(const application_t* application)
{
    for(uint32_t i = 0; i < application->swapchain_details.surface_formats_size; i++)
    {
        VkSurfaceFormatKHR format = application->swapchain_details.vk_surface_formats[i];
        if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    //If we can't have nice things, try to get any at all (try to return the first value)
    if(application->swapchain_details.surface_formats_size > 0)
    {
        return application->swapchain_details.vk_surface_formats[0];
    }

    printf("Couldn't get surface format.\n");
    exit(1);
}

VkPresentModeKHR get_present_mode(const application_t* application)
{
    for(uint32_t i = 0; i < application->swapchain_details.present_modes_size; i++)
    {
        VkPresentModeKHR present_mode = application->swapchain_details.vk_surface_present_modes[i];
        if(present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D get_swap_extent(const application_t* application)
{
    //VkSurfaceCapabilitiesKHR capabilities = application->swapchain_details.vk_surface_capabilities;
    //if(application->swapchain_details.vk_surface_capabilities.currentExtent.width != UINT32_MAX)
    //{
    //    return application->swapchain_details.vk_surface_capabilities.currentExtent;
    //}

    //Don't really know if this is any good/how to test it.
    VkExtent2D extent;
    extent.width = application->window_width;
    extent.height = application->window_height;
    //extent.width =  max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, extent.width));
    //extent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, extent.height));
    return extent;
}

VkSwapchainKHR get_swapchain(const application_t* application)
{
    VkSwapchainKHR vk_swapchain;

    //request one more than min without exceeding max
    uint32_t imageCount = application->swapchain_details.vk_surface_capabilities.minImageCount + 1;
    if (application->swapchain_details.vk_surface_capabilities.maxImageCount > 0 && imageCount > application->swapchain_details.vk_surface_capabilities.maxImageCount)
    {
        imageCount = application->swapchain_details.vk_surface_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = application->vk_surface;
    create_info.minImageCount = imageCount;
    create_info.imageFormat = application->vk_surface_format.format;
    create_info.imageColorSpace = application->vk_surface_format.colorSpace;
    create_info.imageExtent = application->vk_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.flags = 0;
    create_info.pNext = NULL;

    uint32_t indices[2];
    indices[0] = application->queue_family_indices.graphics_family_index;
    indices[1] = application->queue_family_indices.present_family_index;


    if (application->queue_family_indices.graphics_family_index != application->queue_family_indices.present_family_index)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = application->swapchain_details.vk_surface_capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = application->vk_present_mode;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(application->vk_device, &create_info, NULL, &vk_swapchain) != VK_SUCCESS)
    {
        printf("failed to create swap chain.\n");
        exit(1);
    }
    else
    {
        printf("Created swap chain.\n");
        return vk_swapchain;
    }

    //vkGetSwapchainImagesKHR(application->vk_device, vk_swapchain, &imageCount, NULL);
    //swapChainImages.resize(imageCount);
    //vkGetSwapchainImagesKHR(application->vk_device, vk_swapchain, &imageCount, swapChainImages.data());
    //
    //swapChainImageFormat = surfaceFormat.format;
    //swapChainExtent = extent;
}

VkImage* get_swapchain_images(const application_t* application, uint32_t* image_count)
{
    vkGetSwapchainImagesKHR(application->vk_device, application->vk_swapchain, image_count, NULL);
    VkImage* vk_images = (VkImage*)malloc(sizeof(VkImage) * (*image_count));
    vkGetSwapchainImagesKHR(application->vk_device, application->vk_swapchain, image_count, vk_images);
    return vk_images;
}

VkImageView* get_image_views(const application_t* application)
{

    VkImageView* image_views = (VkImageView*)malloc(sizeof(VkImageView) * application->swapchain_images_size);

    for(uint32_t i = 0; i < application->swapchain_images_size; i++)
    {
        VkImageViewCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = NULL;
        createInfo.image = application->vk_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = application->vk_surface_format.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(application->vk_device, &createInfo, NULL, &image_views[i]) != VK_SUCCESS) {
            printf("failed to create image views\n");
            exit(1);
        }
    }
    return image_views;
}

enum type
{
    fragment,
    vertex,
};

VkShaderModule get_shader_module(const application_t* application, const char* path, enum type stype)
{
    //Read the shader file into a char buffer
    unsigned char* file_buffer;
    unsigned long file_size;
    read_file(path, &file_buffer, &file_size);

    //There are a couple caveats
    //We need the code and it's size
    //the size will be in bytes (which is already stored in file_size)
    //the code will be an uint32_t pointer instead of a char
    //That means we have to convert the data properly

    //Code size represents the amount of uint32_t's we need
    //char is guaranteed to be a byte by the spec
    size_t code_size =  file_size/sizeof(uint32_t);
    uint32_t* shader_code = (uint32_t*)malloc(code_size * sizeof(uint32_t));

    for(size_t i = 0; i < code_size; i++)
    {
        //Calculate the proper index into the char buffer
        size_t start_index = i * sizeof(uint32_t);

        //Reinterpret the data in the char buffer as a uint32_t
         shader_code[i] =   (uint32_t) file_buffer[start_index+0]         |
                            (uint32_t) file_buffer[start_index+1] << 8u   |
                            (uint32_t) file_buffer[start_index+2] << 16u  |
                            (uint32_t) file_buffer[start_index+3] << 24u ;
    }

    free(file_buffer);

    //Create the shader module
    VkShaderModule vk_shader_module;
    VkShaderModuleCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    //apparently the codesize is in bytes.
    create_info.codeSize = file_size;
    create_info.pCode = shader_code;
    create_info.flags = 0;
    create_info.pNext = NULL;

    if (vkCreateShaderModule(application->vk_device, &create_info, NULL, &vk_shader_module) != VK_SUCCESS) {
        printf("failed to create shader module!\n");
        exit(1);
    }

    //free the 2nd buffer, now that the shader module has been created
    free(shader_code);

    return vk_shader_module;
}


void get_pipeline_layout_and_pipeline(const application_t* application, VkPipelineLayout* vk_pipeline_layout, VkPipeline* vk_graphics_pipeline)
{
    //Get the shader modules
    VkShaderModule vertex_shader_module = get_shader_module(application, "C:\\projects\\CVulkan\\shaders\\vert.spv", vertex);
    VkShaderModule fragment_shader_module = get_shader_module(application, "C:\\projects\\CVulkan\\shaders\\frag.spv", fragment);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertex_shader_module;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.flags = 0;
    vertShaderStageInfo.pNext = VK_NULL_HANDLE;
    vertShaderStageInfo.pSpecializationInfo = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragment_shader_module;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.flags = 0;
    fragShaderStageInfo.pNext = VK_NULL_HANDLE;
    fragShaderStageInfo.pSpecializationInfo = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertShaderStageInfo, fragShaderStageInfo};

    VkVertexInputBindingDescription vertex_input_description;
    vertex_input_description.binding = 0;
    vertex_input_description.stride = sizeof(vertex_t);
    vertex_input_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_attribute_descriptions[2];
    vertex_attribute_descriptions[0].binding = 0;
    vertex_attribute_descriptions[0].location = 0;
    vertex_attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attribute_descriptions[0].offset = offsetof(vertex_t, pos);

    vertex_attribute_descriptions[1].binding = 0;
    vertex_attribute_descriptions[1].location = 1;
    vertex_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attribute_descriptions[1].offset = offsetof(vertex_t, color);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.flags = 0;
    vertexInputInfo.pNext = 0;

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertex_input_description;

    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertex_attribute_descriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    inputAssembly.flags = 0;
    inputAssembly.pNext = VK_NULL_HANDLE;

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) application->vk_extent.width;
    viewport.height = (float) application->vk_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = application->vk_extent;

    VkPipelineViewportStateCreateInfo viewportState;
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    viewportState.flags = 0;
    viewportState.pNext = VK_NULL_HANDLE;

    VkPipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;

    //rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;


    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    rasterizer.flags = 0;
    rasterizer.pNext = VK_NULL_HANDLE;

    VkPipelineMultisampleStateCreateInfo multisampling;
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = VK_NULL_HANDLE; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    multisampling.flags = 0;
    multisampling.pNext = VK_NULL_HANDLE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending;
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
    colorBlending.flags = 0;
    colorBlending.pNext = VK_NULL_HANDLE;

    //VkDynamicState dynamicStates[] = {
    //        VK_DYNAMIC_STATE_VIEWPORT,
    //        VK_DYNAMIC_STATE_LINE_WIDTH
    //};

    //VkPipelineDynamicStateCreateInfo dynamicState;
    //dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    //dynamicState.dynamicStateCount = 2;
    //dynamicState.pDynamicStates = dynamicStates;
    //dynamicState.flags = 0;
    //dynamicState.pNext = VK_NULL_HANDLE;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &application->vk_descriptor_set_layout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = VK_NULL_HANDLE; // Optional
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.pNext = VK_NULL_HANDLE;



    if (vkCreatePipelineLayout(application->vk_device, &pipelineLayoutInfo, VK_NULL_HANDLE, vk_pipeline_layout) != VK_SUCCESS) {
        printf("failed to create pipeline layout!\n");
        exit(1);
    }
    printf("Created pipeline layout\n");


    //Now setup the pipeline itself
    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.flags = 0;
    pipelineInfo.pNext = VK_NULL_HANDLE;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = VK_NULL_HANDLE; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = VK_NULL_HANDLE; // Optional

    pipelineInfo.layout = *vk_pipeline_layout;

    pipelineInfo.renderPass = application->vk_render_pass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(
            application->vk_device,     //VkDevice device
            VK_NULL_HANDLE,             //VkPipelineCache pipelineCache
            1,                          //uint32_t createInfoCount
            &pipelineInfo,              //const VkGraphicsPipelineCreateInfo *pCreateInfos
            VK_NULL_HANDLE,             //const VkAllocationCallbacks *pAllocator
            vk_graphics_pipeline       //VkPipeline *pPipelines
            ) != VK_SUCCESS) {
        printf("failed to create graphics pipeline.\n");
    }
    printf("Created pipeline\n");

    //Destroy the shader modules
    vkDestroyShaderModule(application->vk_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(application->vk_device, vertex_shader_module, NULL);
}

VkRenderPass get_render_pass(const application_t* application)
{
    VkAttachmentDescription color_attachment;
    color_attachment.format = application->vk_surface_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.flags = 0;

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.preserveAttachmentCount = 0;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = NULL;
    subpass.pPreserveAttachments = NULL;
    subpass.pInputAttachments = NULL;

    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo render_pass_info;
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.flags = 0;
    render_pass_info.pNext = NULL;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VkRenderPass vk_render_pass;
    if (vkCreateRenderPass(application->vk_device, &render_pass_info, NULL, &vk_render_pass) != VK_SUCCESS)
    {
        printf("failed to create render pass\n");
        exit(1);
    }

    printf("created renderpass.\n");
    return vk_render_pass;
}

VkFramebuffer* get_frame_buffers(const application_t* application)
{
    VkFramebuffer* vk_frame_buffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * application->swapchain_images_size);

    for(size_t i = 0; i < application->swapchain_images_size; i++)
    {
        VkImageView attachments[] = {application->vk_image_views[i]};

        VkFramebufferCreateInfo framebuffer_info;
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = application->vk_render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = application->vk_extent.width;
        framebuffer_info.height = application->vk_extent.height;
        framebuffer_info.layers = 1;
        framebuffer_info.flags = 0;
        framebuffer_info.pNext = VK_NULL_HANDLE;

        if (vkCreateFramebuffer(application->vk_device, &framebuffer_info, VK_NULL_HANDLE, &vk_frame_buffers[i]) != VK_SUCCESS) {
            printf("failed to create framebuffer!\n");
        }
    }
    printf("Created framebuffers\n");
    return vk_frame_buffers;
}


VkCommandPool get_command_pool(const application_t* application)
{
    VkCommandPool vk_command_pool;
    VkCommandPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = application->queue_family_indices.graphics_family_index;
    pool_info.flags = 0;
    pool_info.pNext = VK_NULL_HANDLE;

    if (vkCreateCommandPool(application->vk_device, &pool_info, VK_NULL_HANDLE, &vk_command_pool) != VK_SUCCESS) {
        printf("failed to create command pool!\n");
    }
    printf("Created command pool\n");
    return vk_command_pool;
}

VkCommandBuffer* get_command_buffers(const application_t* application)
{
    VkCommandBuffer* vk_command_buffer = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * application->swapchain_images_size);

    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = application->vk_command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = application->swapchain_images_size;
    allocInfo.pNext = VK_NULL_HANDLE;

    if (vkAllocateCommandBuffers(application->vk_device, &allocInfo, vk_command_buffer) != VK_SUCCESS)
    {
        printf("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < application->swapchain_images_size; i++) {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.pInheritanceInfo = VK_NULL_HANDLE;

        if (vkBeginCommandBuffer(vk_command_buffer[i], &beginInfo) != VK_SUCCESS)
        {
            printf("failed to begin recording command buffer!\n");
        }

        VkRenderPassBeginInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = application->vk_render_pass;
        renderPassInfo.framebuffer = application->vk_frame_buffers[i];
        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;
        renderPassInfo.renderArea.extent = application->vk_extent;
        renderPassInfo.pNext = VK_NULL_HANDLE;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        ///vkCmdBeginRenderPass(vk_command_buffer[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
///
        ///////vkCmdBindPipeline(vk_command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, application->vk_graphics_pipeline);////
        ///////vkCmdDraw(vk_command_buffer[i], 3, 1, 0, 0);////
        ///////vkCmdEndRenderPass(vk_command_buffer[i]);
///
///
///
        ///vkCmdBindPipeline(vk_command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, application->vk_graphics_pipeline);
        ///VkBuffer vertexBuffers[] = {application->vk_vertex_buffer};
        ///VkDeviceSize offsets[] = {0};
        ///vkCmdBindVertexBuffers(vk_command_buffer[i], 0, 1, vertexBuffers, offsets);
///
        ///vkCmdDraw(vk_command_buffer[i], 15, 1, 0, 0);
///
        ///if (vkEndCommandBuffer(vk_command_buffer[i]) != VK_SUCCESS)
        ///{
        ///    printf("failed to record command buffer!\n");
        ///}


        vkCmdBeginRenderPass(vk_command_buffer[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(vk_command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, application->vk_graphics_pipeline);

        VkBuffer vertexBuffers[] = {application->vk_vertex_buffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(vk_command_buffer[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(vk_command_buffer[i], application->vk_index_buffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(vk_command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, application->vk_pipeline_layout, 0, 1, &application->vk_descriptor_sets[i], 0, VK_NULL_HANDLE);

        //vkCmdDraw(vk_command_buffer[i], 15, 1, 0, 0);
        vkCmdDrawIndexed(vk_command_buffer[i], 6, 1, 0, 0, 0);



        vkCmdEndRenderPass(vk_command_buffer[i]);

        if (vkEndCommandBuffer(vk_command_buffer[i]) != VK_SUCCESS) {
            printf("failed to record command buffer!\n");
        }
    }


    return vk_command_buffer;
}


VkSemaphore get_semaphore(const application_t* application)
{
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphore_info;
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = 0;
    semaphore_info.pNext = VK_NULL_HANDLE;
    if (vkCreateSemaphore(application->vk_device, &semaphore_info, VK_NULL_HANDLE, &semaphore) != VK_SUCCESS)
    {
        printf("failed to create semaphore!");
    }

    return semaphore;
}

VkFence get_fence(const application_t* application)
{
    VkFence vk_fence;
    VkFenceCreateInfo fence_info;
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;//Create them in signaled state, else calling wait on them will wait forever
    fence_info.pNext = VK_NULL_HANDLE;

    if(vkCreateFence(application->vk_device, &fence_info, NULL, &vk_fence) != VK_SUCCESS) {
        printf("Failed to create fence.");
    }
    return vk_fence;
}

void get_buffer(const application_t* application, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_property_flags, VkBuffer* buffer, VkDeviceMemory* buffer_memory)
{
    VkBufferCreateInfo buffer_info;
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_info.flags = 0;
    buffer_info.pNext = VK_NULL_HANDLE;
    buffer_info.pQueueFamilyIndices = 0;
    buffer_info.queueFamilyIndexCount = 0;


    if (vkCreateBuffer(application->vk_device, &buffer_info, NULL, buffer) != VK_SUCCESS)
    {
        printf("failed to create buffer!\n");
    }

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(application->vk_device, *buffer, &memory_requirements);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(application->vk_physical_device, &memProperties);

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_requirements.size;
    alloc_info.pNext = VK_NULL_HANDLE;
    alloc_info.memoryTypeIndex = find_memory_index(application, memory_requirements.memoryTypeBits, memory_property_flags);

    if (vkAllocateMemory(application->vk_device, &alloc_info, NULL, buffer_memory) != VK_SUCCESS)
    {
        printf("failed to allocate buffer memory!\n");
    }

    vkBindBufferMemory(application->vk_device, *buffer, *buffer_memory, 0);
}

//private helper
void copy_buffer(const application_t* application, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = application->vk_command_pool;
    allocInfo.commandBufferCount = 1;
    allocInfo.pNext = VK_NULL_HANDLE;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(application->vk_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pNext = VK_NULL_HANDLE;
    beginInfo.pInheritanceInfo = NULL;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.pNext = VK_NULL_HANDLE;
    submitInfo.pSignalSemaphores = NULL;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitDstStageMask = NULL;

    vkQueueSubmit(application->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(application->vk_graphics_queue);
    vkFreeCommandBuffers(application->vk_device, application->vk_command_pool, 1, &commandBuffer);
}


void get_vertex_buffer(const application_t* application, vertex_t* vertices, size_t vertices_size, VkBuffer* vertex_buffer, VkDeviceMemory* vertex_buffer_memory)
{
    //Temporary buffers
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize bufferSize = sizeof(vertex_t) * vertices_size;

    get_buffer(application, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(application->vk_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, vertices_size * sizeof(vertex_t));
    vkUnmapMemory(application->vk_device, stagingBufferMemory);

    get_buffer(application, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer, vertex_buffer_memory);

    copy_buffer(application, stagingBuffer, *vertex_buffer, bufferSize);

    vkDestroyBuffer(application->vk_device, stagingBuffer, NULL);
    vkFreeMemory(application->vk_device, stagingBufferMemory, NULL);
}

void get_index_buffer(const application_t* application, uint16_t* indices, size_t indices_size, VkBuffer* index_buffer, VkDeviceMemory* index_buffer_memory) {
    VkDeviceSize bufferSize = sizeof(uint16_t) * indices_size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    get_buffer(application, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(application->vk_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices, (size_t) bufferSize);
    vkUnmapMemory(application->vk_device, stagingBufferMemory);

    get_buffer(application, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer, index_buffer_memory);

    copy_buffer(application, stagingBuffer, *index_buffer, bufferSize);

    vkDestroyBuffer(application->vk_device, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(application->vk_device, stagingBufferMemory, VK_NULL_HANDLE);
}

VkDescriptorSetLayout get_descriptor_set_layout(const application_t* application)
{
    VkDescriptorSetLayout vk_descriptor_set_layout;

    VkDescriptorSetLayoutBinding vk_descriptor_set_layout_binding;
    vk_descriptor_set_layout_binding.binding = 0;
    vk_descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vk_descriptor_set_layout_binding.descriptorCount = 1;
    vk_descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vk_descriptor_set_layout_binding.pImmutableSamplers = VK_NULL_HANDLE; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &vk_descriptor_set_layout_binding;
    layoutInfo.flags = 0;
    layoutInfo.pNext = VK_NULL_HANDLE;

    if (vkCreateDescriptorSetLayout(application->vk_device, &layoutInfo, VK_NULL_HANDLE, &vk_descriptor_set_layout) != VK_SUCCESS) {
        printf("failed to create descriptor set layout!\n");
    }
    printf("Created descriptor set layout\n");
    return vk_descriptor_set_layout;
}

void get_uniform_buffers(const application_t* application, VkBuffer** vk_uniform_buffers, VkDeviceMemory** vk_uniform_buffers_memory, size_t* vk_uniform_buffers_size)
{
    VkDeviceSize bufferSize = sizeof(uniform_buffer_t);
    (*vk_uniform_buffers_size) = application->swapchain_images_size;
    VkBuffer*       uniform_buffer        = (VkBuffer*      )malloc(sizeof(VkBuffer       ) * application->swapchain_images_size);
    VkDeviceMemory* uniform_buffer_memory = (VkDeviceMemory*)malloc(sizeof(VkDeviceMemory ) * application->swapchain_images_size);

    for (size_t i = 0; i < application->swapchain_images_size; i++)
    {
        get_buffer(application, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniform_buffer[i], &uniform_buffer_memory[i]);
    }

    *vk_uniform_buffers = uniform_buffer;
    *vk_uniform_buffers_memory = uniform_buffer_memory;
}

VkDescriptorPool get_descriptor_pool(const application_t* application)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = application->swapchain_images_size;

    VkDescriptorPoolCreateInfo poolInfo;
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.flags = 0;
    poolInfo.pNext = VK_NULL_HANDLE;
    poolInfo.maxSets = application->swapchain_images_size;

    VkDescriptorPool descriptorPool;

    if (vkCreateDescriptorPool(application->vk_device, &poolInfo, VK_NULL_HANDLE, &descriptorPool) != VK_SUCCESS) {
        printf("failed to create descriptor pool!\n");
    }
    return descriptorPool;
}

VkDescriptorSet* get_descriptor_sets(const application_t* application)
{
    VkDescriptorSetLayout layouts[application->swapchain_images_size];
    for(size_t i = 0; i < application->swapchain_images_size; i++)
    {
        layouts[i]= application->vk_descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = application->vk_descriptor_pool;
    allocInfo.descriptorSetCount = application->swapchain_images_size;
    allocInfo.pSetLayouts = layouts;
    allocInfo.pNext = VK_NULL_HANDLE;


    VkDescriptorSet* descriptorSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet) * application->swapchain_images_size);

    if (vkAllocateDescriptorSets(application->vk_device, &allocInfo, descriptorSets) != VK_SUCCESS)
    {
        printf("failed to allocate descriptor sets!\n");
        exit(1);
    }

    for (size_t i = 0; i < application->swapchain_images_size; i++)
    {
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = application->vk_uniform_buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(uniform_buffer_t);

        VkWriteDescriptorSet descriptorWrite;
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.pNext = VK_NULL_HANDLE;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = VK_NULL_HANDLE; // Optional
        descriptorWrite.pTexelBufferView = VK_NULL_HANDLE; // Optional

        vkUpdateDescriptorSets(application->vk_device, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
    }

    return descriptorSets;
}



void createTextureImage(const application_t* application) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("C:\\projects\\CVulkan\\textures\\texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    uint32_t image_size  = texWidth * texHeight * 4;
    VkDeviceSize vk_image_size = (uint64_t)image_size;

    if (!pixels)
    {
        printf("failed to load texture image!");
        exit(1);
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    get_buffer(application, vk_image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(application->vk_device, stagingBufferMemory, 0, vk_image_size, 0, &data);
    memcpy(data, pixels, image_size);
    vkUnmapMemory(application->vk_device, stagingBufferMemory);

    stbi_image_free(pixels);

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageCreateInfo imageInfo;
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texWidth;
    imageInfo.extent.height =texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    imageInfo.pNext = VK_NULL_HANDLE;

    if (vkCreateImage(application->vk_device, &imageInfo, VK_NULL_HANDLE, &textureImage) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(application->vk_device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_index(application, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(application->vk_device, &allocInfo, VK_NULL_HANDLE, &textureImageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(1);
    }

    vkBindImageMemory(application->vk_device, textureImage, textureImageMemory, 0);
}







