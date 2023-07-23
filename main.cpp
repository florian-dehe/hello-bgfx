#include "bgfx.cmake/bgfx/examples/common/bgfx_utils.h"
#include "bgfx.cmake/bx/include/bx/math.h"
#include "glfw/include/GLFW/glfw3.h"
#include <bits/types/FILE.h>
#include <cstddef>
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

// Globals
GLFWwindow *window;
static bgfx::ViewId mainViewId = 0;
static std::string SHADER_FILE_PATH = "assets/shaders/";

// GLFW Callbacks
void error_callback(int error, const char* description){
  fprintf(stderr, "Error: %s\n", description);
}

void resize_callback(GLFWwindow *window, int width, int height) {
  bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
  bgfx::setViewRect(mainViewId, 0, 0, bgfx::BackbufferRatio::Equal);
}

// Rendering
struct PosColorVertex {
  float x;
  float y;
  float z;
  uint32_t agbr;
};

// Cube
static PosColorVertex cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
    0, 1, 2,
    1, 3, 2,
    4, 6, 5,
    5, 6, 7,
    0, 2, 4,
    4, 2, 6,
    1, 5, 3,
    5, 7, 3,
    0, 4, 1,
    4, 5, 1,
    2, 3, 6,
    6, 3, 7,
};

bgfx::ShaderHandle bgfxLoadShader(const char* FILENAME) {
  std::string filepath = SHADER_FILE_PATH + FILENAME;

  FILE *file = fopen(filepath.c_str(), "rb");
  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
  fread(mem->data, 1, mem->size, file);
  mem->data[mem->size + 1] = '\0';
  fclose(file);

  return bgfx::createShader(mem);
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

  glfwSetFramebufferSizeCallback(window, resize_callback);

  // Clear
  bgfx::setViewClear(mainViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355ff, 1.f, 0);
  bgfx::setViewRect(0, 0, 0, W_WIDTH, W_HEIGHT);

  //Rendering some cube
  bgfx::VertexLayout cubeLayout;
  cubeLayout.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
  .end();
  bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), cubeLayout);
  bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

  bgfx::ShaderHandle vsh = bgfxLoadShader("vs_cubes.bin");
  bgfx::ShaderHandle fsh = bgfxLoadShader("fs_cubes.bin");
  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

  // Game Loop
  unsigned int counter = 0;
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    bx::Vec3 eye = {0.f, 0.f, -5.f};
    bx::Vec3 at = {0.f, 0.f, 0.f};
    float view[16];
    bx::mtxLookAt(view, eye, at);
    float proj[16];
    bx::mtxProj(proj, 60.f, float(W_WIDTH) / float(W_HEIGHT), 0.1f, 100.f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(mainViewId, view, proj);

    float transform[16];
    bx::mtxRotateXY(transform, counter * 0.01f, counter * 0.01f);
    bgfx::setTransform(transform);

    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);

    bgfx::submit(mainViewId, program);
    bgfx::frame();
    counter++;
  }

  // Shutdown gracefully
  bgfx::destroy(vbh);
  bgfx::destroy(ibh);
  bgfx::shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
