// clang-format off
#include <common/Window.hpp>
#include <stdint.h>                  // for uint32_t
#include <common/Instance.hpp>       // for Instance
#include <stdexcept>                 // for runtime_error
#include <GLFW/glfw3.h>              // for glfwCreateWindow, glfwCreateWind...
#include <glm/detail/type_vec2.hpp>  // for vec<>::(anonymous)
// clang-format on

using namespace vkl;

Window::Window(const glm::ivec2& dimensions, const std::string& title, const Instance& instance)
    : m_dimensions(dimensions),
      m_title(title),
      m_instance(instance),
      m_surface(VK_NULL_HANDLE),
      m_framebufferResized(true),
      m_drawFrameFunc([](bool&) {}) {
  m_window = glfwCreateWindow(dimensions.x, dimensions.y, title.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);

  if (glfwCreateWindowSurface(instance.handle(), m_window, nullptr, &m_surface) != VK_SUCCESS) {
    throw std::runtime_error("Unable to create window surface");
  }
}

Window::~Window() {
  vkDestroySurfaceKHR(m_instance.handle(), m_surface, nullptr);
  glfwDestroyWindow(m_window);
}

void Window::mainLoop() {
  while (!glfwWindowShouldClose(m_window)) {
    glfwPollEvents();
    m_drawFrameFunc(m_framebufferResized);
  }
}

void Window::GetRequiredExtensions(std::vector<const char*>& out) {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  out.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
  Window* win               = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  win->m_framebufferResized = true;
}