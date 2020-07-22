//
// Created by Frank on 04/05/2020.
//

#ifndef CNES_UTILITIES_H
#define CNES_UTILITIES_H

#include <cglm/cglm.h>

void read_file(const char* file_path, unsigned char** buffer, unsigned long* file_size);

void mat4_zero_initialize(mat4* mat);

#endif //CNES_UTILITIES_H
