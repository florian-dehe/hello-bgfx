cmake_minimum_required(VERSION 3.10)

project(hello-bgfx)

#Set CXX standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Main dependencies
add_subdirectory(bgfx.cmake)
add_subdirectory(glfw)
find_package(Vulkan)

# Options for main dependencies
set(GLFW_USE_WAYLAND TRUE)

# *Our* sources
set(SRCS main.cpp)

# Building final exec & linking libraries
add_executable(hello-bgfx ${SRCS})
target_link_libraries(hello-bgfx bgfx glfw Vulkan::Vulkan)

