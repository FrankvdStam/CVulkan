//
// Created by Frank on 10/07/2020.
//

#include "vulkan.h"


//========================================================================================================================================
//surface

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfw_window = glfwCreateWindow(application->window_with, application->window_height, application->title, NULL, NULL);

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
    VkSurfaceCapabilitiesKHR capabilities = application->swapchain_details.vk_surface_capabilities;
    if(application->swapchain_details.vk_surface_capabilities.currentExtent.width != UINT32_MAX)
    {
        return application->swapchain_details.vk_surface_capabilities.currentExtent;
    }

    //Don't really know if this is any good/how to test it.
    VkExtent2D extent;
    extent.width = application->window_with;
    extent.height = application->window_height;
    extent.width =  max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, extent.width));
    extent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, extent.height));
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

    VkImageView* image_views = (VkImageView*)malloc(sizeof(VkImageView) * application->image_views_buffers_size);

    for(uint32_t i = 0; i < application->image_views_buffers_size; i++)
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

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = VK_NULL_HANDLE; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = VK_NULL_HANDLE; // Optional
    vertexInputInfo.flags = 0;
    vertexInputInfo.pNext = 0;

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
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    dynamicState.flags = 0;
    dynamicState.pNext = VK_NULL_HANDLE;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = VK_NULL_HANDLE; // Optional
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

    VkRenderPassCreateInfo render_pass_info;
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 0;
    render_pass_info.flags = 0;
    render_pass_info.pNext = NULL;
    render_pass_info.pDependencies = NULL;

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
    VkFramebuffer* vk_frame_buffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * application->image_views_buffers_size);

    for(size_t i = 0; i < application->image_views_buffers_size; i++)
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
    VkCommandBuffer* vk_command_buffer = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * application->image_views_buffers_size);

    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = application->vk_command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = application->image_views_buffers_size;
    allocInfo.pNext = VK_NULL_HANDLE;

    if (vkAllocateCommandBuffers(application->vk_device, &allocInfo, vk_command_buffer) != VK_SUCCESS) {
        printf("failed to allocate command buffers!");
    }
    return vk_command_buffer;
}