#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

namespace vkl {

  namespace misc {

    /**
     * @brief Get the index of a memory type that has all the requested property bits set
     * @param typeFilter Indique les types de mémoire que l'on veut trouver (chaque n-ième
     * correspond a un type de mémoire)
     * @param properties Bitmask spécifiant les propriétés d'un type de mémoire
     * @return Indice de la mémoire
     * @throw Throws an exception if no memory type could be found that supports the requested properties
     */
    inline uint32_t findMemoryType(VkPhysicalDevice physicalDevice,
                                   uint32_t typeFilter,
                                   VkMemoryPropertyFlags properties) {
      // On récupère les différents type de mémoire que la carte graphique peut offir
      VkPhysicalDeviceMemoryProperties memProperties;
      vkGetPhysicalDeviceMemoryProperties(
          physicalDevice,
          &memProperties);  // Reports memory information for the specified physical device

      // La structure VkPhysicalDeviceMemoryProperties comprend deux tableaux appelés memoryHeaps et
      // memoryTypes

      for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        // On itére sur les bits de typeFilter pour trouver les types de mémoire qui lui correspondent
        if ((typeFilter & (1 << i))
            // On vérifie que la mémoire est accesible, properyFlags doit comprend au moins
            // VK_MEMORY_PROPERTY_HOSY_VISIBLE_BIT et VK_MEMORY_PROPERTY_HOSY_COHERENT_BIT
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
          return i;
        }
      }

      throw std::runtime_error("aucun type de memoire ne satisfait le buffer!");
    }

    // Returns if a given format support LINEAR filtering
    inline VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling) {
      VkFormatProperties formatProps;
      vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

      if (tiling == VK_IMAGE_TILING_OPTIMAL)
        return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

      if (tiling == VK_IMAGE_TILING_LINEAR)
        return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

      return false;
    }

  }  // namespace misc

}  // namespace vkl
