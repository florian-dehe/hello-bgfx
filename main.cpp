#include "glfw/include/GLFW/glfw3.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_X11
#include "GLFW/glfw3native.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/defines.h"

#define W_WIDTH 1600
#define W_HEIGHT 900

GLFWwindow *window;
static bgfx::ViewId mainViewId = 0;

void error_callback(int error, const char* description){
  fprintf(stderr, "Error: %s\n", description);
}

void resize_callback(GLFWwindow *window, int width, int height) {
  bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
  bgfx::setViewRect(mainViewId, 0, 0, bgfx::BackbufferRatio::Equal);
}

int main(int argc, char *argv[]) {

  if (!glfwInit()) {
    std::cerr << "GLFW Init Failed !\n";
    return -1;
  }

  if (!glfwVulkanSupported()) {
    std::cerr << "Vulkan not supported !\n";
    return -1;
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Hello, bgfx!", NULL, NULL);
  if (!window) {
    std::cerr << "GLFW Window creation failed !\n";
    return -1;
  }

  bgfx::Init bgfxInit;
  // Graphics Lib
  bgfxInit.type = bgfx::RendererType::Vulkan;

  // Wayland
  // bgfxInit.platformData.nwh = (void*) (std::uintptr_t) glfwGetWaylandWindow(window);
  // bgfxInit.platformData.ndt = glfwGetWaylandDisplay();

  // X11
  bgfxInit.platformData.nwh = (void*) (std::uintptr_t) glfwGetX11Window(window);
  bgfxInit.platformData.ndt = glfwGetX11Display();

  // Resolution
  bgfxInit.resolution.width = W_WIDTH;
  bgfxInit.resolution.height = W_HEIGHT;
  bgfxInit.resolution.reset = BGFX_RESET_VSYNC;

  bgfx::init(bgfxInit);

  //
  glfwSetFramebufferSizeCallback(window, resize_callback);

  // Clear
  bgfx::setViewClear(mainViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355ff, 1.f, 0);
  bgfx::setViewRect(0, 0, 0, W_WIDTH, W_HEIGHT);

  // Game Loop
  unsigned int counter = 0;
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    bgfx::touch(mainViewId);

    bgfx::frame();
    counter++;
  }

  // Shutdown gracefully
  bgfx::shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
