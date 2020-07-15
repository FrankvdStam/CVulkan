//
// Created by Frank on 06/07/2020.
//

#include "application.h"

#include "rendering/extensions.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "rendering/vulkan.h"



const int MAX_FRAMES_IN_FLIGHT = 2;

//========================================================================================================================================
//Private

void drawFrame(application_t* application)
{
    vkWaitForFences(application->vk_device, 1, &application->vk_fences[application->current_frame], VK_TRUE, UINT64_MAX);
    vkResetFences(application->vk_device, 1, &application->vk_fences[application->current_frame]);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(application->vk_device, application->vk_swapchain, UINT64_MAX, application->vk_image_available_semaphore[application->current_frame], VK_NULL_HANDLE, &imageIndex);

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (application->vk_image_in_flight_fences[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(application->vk_device, 1, &application->vk_image_in_flight_fences[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    application->vk_image_in_flight_fences[imageIndex] = application->vk_fences[application->current_frame];


    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        application_rebuild_swapchain(application);
        printf("Rebuilt swapchain.\n");
        return;
    }

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = VK_NULL_HANDLE;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &application->vk_image_available_semaphore[application->current_frame];
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &application->vk_command_buffers[imageIndex];

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &application->vk_render_finished_semaphore[application->current_frame];

    if (vkQueueSubmit(application->vk_graphics_queue, 1, &submitInfo, application->vk_fences[application->current_frame]) != VK_SUCCESS) {
        printf("failed to submit draw command buffer!\n");
    }



    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &application->vk_render_finished_semaphore[application->current_frame];
    presentInfo.pNext = VK_NULL_HANDLE;

    VkSwapchainKHR swapChains[] = {application->vk_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = NULL; // Optional

    vkQueuePresentKHR(application->vk_present_queue, &presentInfo);
    vkQueueWaitIdle(application->vk_present_queue);

    application->current_frame = (application->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    static int frames = 0;
    printf("frame: %i, iff: %zu, ii: %u\n", frames++, application->current_frame, imageIndex);
}


//========================================================================================================================================
//Cleanup
void application_cleanup(application_t* application)
{
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(application->vk_device, application->vk_image_available_semaphore[i], NULL);
        vkDestroySemaphore(application->vk_device, application->vk_render_finished_semaphore[i], NULL);
        vkDestroyFence(application->vk_device, application->vk_fences[i], NULL);
    }
    free(application->vk_image_available_semaphore);
    free(application->vk_render_finished_semaphore);
    free(application->vk_fences);
    free(application->vk_image_in_flight_fences);

    free(application->vk_command_buffers);

    vkDestroyCommandPool(application->vk_device, application->vk_command_pool, NULL);

    for(size_t i = 0; i < application->image_views_buffers_size; i++)
    {
        vkDestroyFramebuffer(application->vk_device, application->vk_frame_buffers[i], NULL);
    }
    free(application->vk_frame_buffers);

    vkDestroyPipeline(application->vk_device, application->vk_graphics_pipeline, NULL);
    vkDestroyPipelineLayout(application->vk_device, application->vk_pipeline_layout, NULL);
    vkDestroyRenderPass(application->vk_device, application->vk_render_pass, NULL);
    for(uint32_t i = 0; i < application->image_views_buffers_size; i++)
    {
        vkDestroyImageView(application->vk_device, application->vk_image_views[i], NULL);
    }

    free(application->vk_images);
    vkDestroySwapchainKHR(application->vk_device, application->vk_swapchain, NULL);

    free(application->swapchain_details.vk_surface_present_modes);
    free(application->swapchain_details.vk_surface_formats);

    if(application->vulkan_debugging_mode == vulkan_debugging_enabled)
    {
        free_debug_utils_messenger_extension(application);
    }
    vkDestroySurfaceKHR(application->vk_instance, application->vk_surface, NULL);
    vkDestroyDevice(application->vk_device, NULL);
    vkDestroyInstance(application->vk_instance, NULL);

    string_list_free(application->required_extension_names);
    string_list_free(application->required_layer_names);

    glfwDestroyWindow(application->glfw_window);
    glfwTerminate();

    free(application->title);
    free(application);
}

//========================================================================================================================================
//public

application_t* application_init(int window_with, int window_height, char* title, vulkan_debugging_mode_t vulkan_debugging_mode)
{
    application_t* application = (application_t*)malloc(sizeof(application_t));
    application->window_with = window_with;
    application->window_height = window_height;
    application->title = (char*)malloc(sizeof(char) * strlen(title));
    strcpy(application->title, title);
    application->current_frame = 0;
    application->vulkan_debugging_mode = vulkan_debugging_mode;

    //Initialize glfw and get a window
    application->glfw_window = glfw_init_get_window(application);

    //now that glfw is initialized, we can ask it about what extensions it needs
    application->required_extension_names = get_required_extensions(application);
    application->required_layer_names = get_required_layers(application);

    //Can now initialize vulkan
    application->vk_instance                    = get_instance(application);
    application->vk_debug_messenger             = get_debug_callback(application);
    application->vk_surface                     = get_vk_surface(application);
    application->vk_physical_device             = get_physical_device(application);
    application->queue_family_indices           = get_queue_family_indices(application);
    application->vk_device                      = get_logical_device(application);
    application->vk_graphics_queue              = get_graphics_queue(application);
    application->vk_present_queue               = get_present_queue(application);
    application->swapchain_details              = get_swapchain_details(application);
    application->vk_surface_format              = get_surface_format(application);
    application->vk_present_mode                = get_present_mode(application);
    application->vk_extent                      = get_swap_extent(application);
    application->vk_swapchain                   = get_swapchain(application);
    application->vk_images                      = get_swapchain_images(application, &application->image_views_buffers_size);
    application->vk_image_views                 = get_image_views(application);
    application->vk_render_pass                 = get_render_pass(application);
    get_pipeline_layout_and_pipeline(application, &application->vk_pipeline_layout, &application->vk_graphics_pipeline);
    application->vk_frame_buffers               = get_frame_buffers(application);
    application->vk_command_pool                = get_command_pool(application);
    application->vk_command_buffers             = get_command_buffers(application);

    application->vk_image_available_semaphore = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    application->vk_render_finished_semaphore = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    application->vk_fences                    = (VkFence*)    malloc(sizeof(VkFence)     * MAX_FRAMES_IN_FLIGHT);
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        application->vk_image_available_semaphore[i] = get_semaphore(application);
        application->vk_render_finished_semaphore[i] = get_semaphore(application);
        application->vk_fences[i]                    = get_fence(application);
    }

    application->vk_image_in_flight_fences    = (VkFence*)    malloc(sizeof(VkFence)     * application->image_views_buffers_size);
    for(size_t i = 0; i < application->image_views_buffers_size; i++)
    {
        application->vk_image_in_flight_fences[i] = VK_NULL_HANDLE;
    }
    return application;
}


void application_run(application_t* application)
{
    while (!glfwWindowShouldClose(application->glfw_window)) {
        glfwPollEvents();
        //glfwSetWindowShouldClose(application->glfw_window, 1);
        drawFrame(application);


    }
    vkDeviceWaitIdle(application->vk_device);
    application_cleanup(application);
}


void application_rebuild_swapchain(application_t* application)
{
    int width, height;
    glfwGetFramebufferSize(application->glfw_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(application->glfw_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(application->vk_device);

    //First cleanup everything related to the current swapchain that we can't reuse later
    for (size_t i = 0; i < application->image_views_buffers_size; i++) {
        vkDestroyFramebuffer(application->vk_device, application->vk_frame_buffers[i], NULL);
    }

    vkFreeCommandBuffers(application->vk_device, application->vk_command_pool, application->image_views_buffers_size,  application->vk_command_buffers);
    vkDestroyPipeline(application->vk_device, application->vk_graphics_pipeline, NULL);
    vkDestroyPipelineLayout(application->vk_device, application->vk_pipeline_layout, NULL);
    vkDestroyRenderPass(application->vk_device, application->vk_render_pass, NULL);

    for (size_t i = 0; i < application->image_views_buffers_size; i++) {
        vkDestroyImageView(application->vk_device, application->vk_image_views[i], NULL);
    }

    vkDestroySwapchainKHR(application->vk_device, application->vk_swapchain, NULL);


    //Now rebuild
    application->window_with = width;
    application->window_height = height;

    application->vk_extent                      = get_swap_extent(application);
    application->vk_swapchain                   = get_swapchain(application);
    application->vk_images                      = get_swapchain_images(application, &application->image_views_buffers_size);
    application->vk_image_views                 = get_image_views(application);
    application->vk_render_pass                 = get_render_pass(application);
    get_pipeline_layout_and_pipeline(application, &application->vk_pipeline_layout, &application->vk_graphics_pipeline);
    application->vk_frame_buffers               = get_frame_buffers(application);
    application->vk_command_buffers             = get_command_buffers(application);
}





