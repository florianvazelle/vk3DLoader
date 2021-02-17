/**
 * @file DebugUtilsMessenger.hpp
 * @brief Define DebugUtilsMessenger class
 */

#ifndef DEBUGUTILSMESSENGER_HPP
#define DEBUGUTILSMESSENGER_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>

namespace vkl {

  class Instance;

  /**
   * @brief A class that allows you to manage debugging messages, generated by the validation layer.
   */
  class DebugUtilsMessenger : public NonCopyable {
  public:
    explicit DebugUtilsMessenger(const Instance& instance, bool exitOnError);
    ~DebugUtilsMessenger() noexcept(false);

    inline const VkDebugUtilsMessengerEXT& handle() const { return m_debugMessenger; }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  private:
    VkDebugUtilsMessengerEXT m_debugMessenger;
    bool m_exitOnError;

    const Instance& m_instance;
  };

}  // namespace vkl

#endif  // DEBUGUTILSMESSENGER_HPP