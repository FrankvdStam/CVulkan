//
// Created by Frank on 07/07/2020.
//

#include "extensions.h"

//==========================================================================================================
//Store all the different types of extensions so we only have to fetch them once

unsigned int layer_count;
VkLayerProperties* layer_properties;

unsigned int extension_count = 0;
VkExtensionProperties* extension_properties;

bool extensions_loaded = false;
void load_all_extensions()
{
    if(extensions_loaded)
    {
        return;
    }
    extensions_loaded = true;

    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    layer_properties = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    extension_properties = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extension_count);
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extension_properties);
}

//==========================================================================================================
//api

bool extension_exists(const char* extension_name)
{
    load_all_extensions();
    for(size_t i = 0; i < extension_count; i++)
    {
        if(strcmp(extension_name, extension_properties[i].extensionName) == 0)
        {
            return true;
        }
    }
    return false;
}

bool layer_exists(const char* extension_name)
{
    load_all_extensions();

    return false;
}


void print_all_extensions()
{
    load_all_extensions();

    printf("Available layers:\n");
    for(unsigned int i = 0; i < layer_count; i++)
    {
        printf("%s\n", layer_properties[i].layerName);
    }

    printf("Available extensions:\n");
    for(unsigned int i = 0; i < extension_count; i++)
    {
        printf("%s\n", extension_properties[i].extensionName);
    }
}