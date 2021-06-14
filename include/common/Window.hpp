/**
 * @file Window.hpp
 * @brief Define Window class
 */

#ifndef WINDOW_HPP
#define WINDOW_HPP

// clang-format off
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>      // for VkSurfaceKHR, VkSurfaceKHR_T
#include <common/NoCopy.hpp>           // for NoCopy
#include <functional>                // for function
#include <string>                    // for string
#include <vector>                    // for vector
#include <glm/glm.hpp>               // for ivec2, vec<>::operator[]
namespace vkl { class Instance; }
// clang-format on

namespace vkl {

  class Instance;

  /**
   * @brief A class which allows to manage a surface, with GLFW.
   */
  class Window : public NoCopy {
  public:
    // using DrawFrameFunc = void(*)(bool& framebufferResized);

    Window(const glm::ivec2& dimensions, const std::string& title, const vkl::Instance& instance);
    Window() = delete;
    ~Window();

    /**
     * @brief Event management loop
     * A classic of any program using GLFW, which will allow us to run our application until an error or other event
     * interrupts it.
     * We call the method to draw a frame here.
     */
    void mainLoop();

    inline const glm::ivec2& dimensions() const { return m_dimensions; }

    inline const GLFWwindow* window() const { return m_window; }
    inline GLFWwindow* window() { return m_window; }

    inline const VkSurfaceKHR& surface() const { return m_surface; }

    inline void framebufferSize(glm::ivec2& size) const { glfwGetFramebufferSize(m_window, &size[0], &size[1]); }

    inline void setDrawFrameFunc(const std::function<void(bool&)>& func) { m_drawFrameFunc = func; }

    static void GetRequiredExtensions(std::vector<const char*>& out);

  private:
    GLFWwindow* m_window;

    glm::ivec2 m_dimensions;
    std::string m_title;

    const vkl::Instance& m_instance;
    VkSurfaceKHR m_surface;

    bool m_framebufferResized;
    std::function<void(bool&)> m_drawFrameFunc;

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
  };

}  // namespace vkl

#endif  // WINDOW_HPP