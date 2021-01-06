#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

namespace vkl {
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;

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

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
      /**
       * Note Exposé : Facile, on rajoute un VkVertexInputAttributeDescription pour chaque element de notre structure
       * vertex. Ici, notre premier element décrit donc la position, le deuxième, la couleur ...
       * Comme en openGL avec glVertexAttribPointer.
       */

      std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
          {
              .location = 0,  // Correspond à la valeur donnée à la directive location dans le code du vertex shader
              .binding  = 0,
              .format   = VK_FORMAT_R32G32B32_SFLOAT,  // Décrit le type de donnée de l'attribut
              .offset   = offsetof(Vertex, pos),       // Indique de combien d'octets il faut se décaler
          },
          // normal
          {
              .location = 1,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32B32_SFLOAT,
              .offset   = offsetof(Vertex, normal),
          },
          // color
          {
              .location = 2,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32B32_SFLOAT,  // vec3
              .offset   = offsetof(Vertex, color),
          },
          // texCoord
          {
              .location = 3,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32_SFLOAT,  // vec2
              .offset   = offsetof(Vertex, texCoord),
          },
      };

      return attributeDescriptions;
    }
  };

}  // namespace vkl

#endif  // VERTEX_HPP