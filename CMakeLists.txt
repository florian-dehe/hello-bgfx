cmake_minimum_required(VERSION 3.10)

project(hello-bgfx)

#Set CXX standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Main dependencies
add_subdirectory(bgfx.cmake)

# GLFW from sources
# add_subdirectory(glfw)
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(glfw3 3.3 REQUIRED)
# set(GLFW_USE_WAYLAND TRUE)

# set(OpenGL_GL_PREFERENCE GLVND)
# find_package(OpenGL)
find_package(Vulkan)


# *Our* sources
set(SRCS main.cpp)

# Building final exec & linking libraries
add_executable(hello-bgfx ${SRCS})
target_link_libraries(hello-bgfx bgfx glfw Vulkan::Vulkan)
