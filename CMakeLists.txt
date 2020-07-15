cmake_minimum_required(VERSION 3.13)
project(CVulkan C)

set(CMAKE_C_STANDARD 11)

add_executable(CVulkan src/main.c src/common.h src/utilities.c src/utilities.h src/application.h src/application.c src/rendering/extensions.c src/rendering/extensions.h src/utility/string_list.c src/utility/string_list.h src/rendering/vulkan.c src/rendering/vulkan.h)

include_directories(C:/VulkanSDK/1.2.141.2/include/)

add_subdirectory (lib/glfw)
add_subdirectory(lib/cglm/)

ADD_LIBRARY(vulkanlib STATIC IMPORTED)
SET_TARGET_PROPERTIES(vulkanlib PROPERTIES IMPORTED_LOCATION C:/VulkanSDK/1.2.141.2/Lib/vulkan-1.lib)

execute_process(
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/shaders/build.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)

target_link_libraries(CVulkan vulkanlib)
target_link_libraries(CVulkan glfw)
target_link_libraries(CVulkan cglm)