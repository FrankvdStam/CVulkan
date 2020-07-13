#include "common.h"
#include "application.h"
#include "utilities.h"


int main(int argc, char *argv[])
{
    application_t* app = application_init(800, 600, "Vulkan", vulkan_debugging_enabled);
    //application_t* app = application_init(800, 600, "Vulkan", vulkan_debugging_disabled);
    application_run(app);
    return 0;
}