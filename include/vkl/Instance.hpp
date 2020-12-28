#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <stdexcept>
#include <vector>

#include <NonCopyable.hpp>

namespace vkl {

  /**
   * Note Exposé : Un VkInstance est un objet qui contient toutes les informations dont notre
   * application Vulkan a besoin pour fonctionner. Contrairement à OpenGL, Vulkan n'a pas d'état
   * global. Pour cette raison, nous devons à la place stocker nos états dans cet objet.
   */

  class Instance : public NonCopyable {
  public:
    Instance(const char* appName, const char* engineName, bool validationLayers);
    Instance() = delete;
    ~Instance();

    inline const VkInstance& handle() const { return m_instance; }
    inline bool validationLayersEnabled() const { return m_enableValidationLayers; }

    static const std::vector<const char*> ValidationLayers;
    static const std::vector<const char*> DeviceExtensions;

  private:
    VkInstance m_instance;
    bool m_enableValidationLayers;

    static bool CheckValidationLayerSupport();
    static void GetRequiredExtensions(std::vector<const char*>& extensions, bool validationLayers);
  };

}  // namespace vkl

#endif  // INSTANCE_HPP