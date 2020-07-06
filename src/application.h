//
// Created by Frank on 06/07/2020.
//

#ifndef CVULKAN_APPLICATION_H
#define CVULKAN_APPLICATION_H

#include "common.h"

void run();
void init_vulkan();
void main_loop();
void cleanup();

void create_instance();
void setup_debug_message_callback();
#endif //CVULKAN_APPLICATION_H
