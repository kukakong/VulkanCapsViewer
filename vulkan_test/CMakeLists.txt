cmake_minimum_required(VERSION 3.10)
project(vulkan_caps_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Vulkan SDK
# Supports VULKAN_SDK environment variable and CMAKE_TOOLCHAIN_FILE for cross-compilation
find_package(Vulkan REQUIRED)

# Main executable
add_executable(vulkan_caps_test vulkan_caps_test.cpp)

target_include_directories(vulkan_caps_test PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan_caps_test PRIVATE Vulkan::Vulkan)

# Cross-compilation support: propagate toolchain settings
# When cross-compiling for ARM/Mali, use:
#   cmake -DCMAKE_TOOLCHAIN_FILE=<toolchain>.cmake ..
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling for: ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})")
endif()

# Installation rules
install(TARGETS vulkan_caps_test RUNTIME DESTINATION bin)
