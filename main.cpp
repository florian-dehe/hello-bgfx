#include <cstdio>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include "GLFW/glfw3native.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/defines.h"

#define W_WIDTH 1600
#define W_HEIGHT 900

GLFWwindow *window;

void error_callback(int error, const char* description){
  fprintf(stderr, "Error: %s\n", description);
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

  // Passing native window to bgfx & init
  bgfx::PlatformData pd;
  pd.nwh = glfwGetWaylandWindow(window);
  bgfx::setPlatformData(pd);

  bgfx::Init bgfxInit;
  bgfxInit.type = bgfx::RendererType::Vulkan;
  bgfxInit.resolution.width = W_WIDTH;
  bgfxInit.resolution.height = W_HEIGHT;
  bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
  if (!bgfx::init()) {
    std::cerr << "BGFX Init failed !\n";
    return -1;
  }

  std::cout << "made it!\n";

  // Clear
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355ff, 1.f, 0);
  std::cout << "made it!\n";
  bgfx::setViewRect(0, 0, 0, W_WIDTH, W_HEIGHT);
  std::cout << "made it!\n";

  // Game Loop
  unsigned int counter = 0;
  while(!glfwWindowShouldClose(window)) {
    bgfx::frame();
    counter++;
  }

  // Shutdown gracefully
  bgfx::shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
