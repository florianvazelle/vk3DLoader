#pragma once

#if defined(_WIN32)
#  include <windows.h>
#else
#  include <iostream>
#endif

/**
 * @brief Display a message and exit the program
 * @param msg Message a afficher
 */
static void exitOnError(const char* msg) {
#if defined(_WIN32)
  MessageBox(NULL, msg, ENGINE_NAME, MB_ICONERROR);  // Pop-up for window user
#else
  std::cout << msg << std::endl;
#endif
  exit(EXIT_FAILURE);
}

#include <vkl/Device.hpp>

/**
 * @brief Déterminer un type de mémoire
 * @param device
 * @param typeFilter Indique les types de mémoire que l'on veut trouver (chaque n-ième correspond a
 * un type de mémoire)
 * @param properties Bitmask spécifiant les propriétés d'un type de mémoire
 * @return Indice de la mémoire
 */
static uint32_t findMemoryType(const vkl::Device& device,
                               uint32_t typeFilter,
                               VkMemoryPropertyFlags properties) {
  // On récupère les différents type de mémoire que la carte graphique peut offir
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(device.physical(), &memProperties);

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