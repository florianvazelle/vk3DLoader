#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <array>
#include <glm/glm.hpp>

namespace vkl {
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    // Renvoie l'instance de cette structure
    static VkVertexInputBindingDescription getBindingDescription() {
      // Un vertex binding décrit la lecture des données stockées en mémoire. Elle fournit le nombre
      // d'octets entre les jeux de données et la manière de passer d'un ensemble de données (par
      // exemple une coordonnée) au suivant. Elle permet à Vulkan de savoir comment extraire chaque
      // jeu de données correspondant à une invocation du vertex shader du vertex buffer.
      VkVertexInputBindingDescription bindingDescription{};
      // Indique l'indice du vertex binding dans le tableau des bindings
      bindingDescription.binding = 0;
      // Fournit le nombre d'octets séparant les débuts de deux ensembles de données,
      bindingDescription.stride = sizeof(Vertex);
      // Passe au jeu de données suivante après chaque sommet
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
      std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
      attributeDescriptions[0].binding = 0;
      // Correspond à la valeur donnée à la directive location dans le code du vertex shader
      attributeDescriptions[0].location = 0;
      // Décrit le type de donnée de l'attribut
      attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;  // vec2
      // Indique de combien d'octets il faut se décaler
      attributeDescriptions[0].offset = offsetof(Vertex, pos);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;  // vec3
      attributeDescriptions[1].offset = offsetof(Vertex, color);

      return attributeDescriptions;
    }
  };

}  // namespace vkl

#endif  // VERTEX_HPP