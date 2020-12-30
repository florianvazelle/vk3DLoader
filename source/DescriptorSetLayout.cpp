#include <vkl/DescriptorSetLayout.hpp>

#include <stdexcept>

using namespace vkl;

DescriptorSetLayout::DescriptorSetLayout(const Device& device) : m_device(device) {
  const VkDescriptorSetLayoutBinding uboLayoutBinding = {
      // valeur indiquée dans le shader
      .binding = 0,
      // type de descripteur (ici uniform buffer)
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      // nombre d'éléments qu'il contient
      .descriptorCount = 1,
      // les différents shader qui accèderont à cette ressource (ici, vertex shader)
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,  // or VK_SHADER_STAGE_ALL_GRAPHICS
  };

  // Les liens des descripteurs doivent être combinés en un seul objet VkDescriptorSetLayout

  // On crée un VkDescriptorSetLayoutCreateInfo pour regrouper les descripteurs
  const VkDescriptorSetLayoutCreateInfo layoutInfo = {
      .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 1,
      .pBindings    = &uboLayoutBinding,  // tableau contenant les structures qui décrivent les bindings
  };

  // On crée le VkDescriptorSetLayout
  if (vkCreateDescriptorSetLayout(m_device.logical(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("echec de la creation d'un set de descripteurs!");
  }
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(m_device.logical(), m_descriptorSetLayout, nullptr);
}