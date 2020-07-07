//
// Created by Frank on 07/07/2020.
//

#ifndef CVULKAN_EXTENSIONS_H
#define CVULKAN_EXTENSIONS_H

#include "common.h"

bool extension_exists(const char* extension_name);
bool layer_exists(const char* extension_name);

void print_all_extensions();

#endif //CVULKAN_EXTENSIONS_H
