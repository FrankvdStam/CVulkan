#include "common.h"
#include "utilities.h"
#include <vulkan/vulkan.h>

#include "application.h"

int main() {
    application_t* app = application_init(800, 600, "Vulkan");
    application_run(app);
    return 0;
}