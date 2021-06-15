#pragma once

#include <common/CommandPool.hpp>
#include <common/Device.hpp>
#include <common/buffer/Buffer.hpp>
#include <common/buffer/StorageBuffer.hpp>
#include <common/struct/Particle.hpp>

#include <random>
#include <vector>

#define NUM_PARTICLE 10

namespace vkl {

  class ParticleStorageBuffer : public StorageBuffer {

    public:
    ParticleStorageBuffer(const Device& device,
                          const CommandPool& commandPool,
                          VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties)
        : StorageBuffer(device, NUM_PARTICLE * sizeof(Particle), usage, properties) {
      //

      std::vector<glm::vec3> attractors = {
          glm::vec3(5.0f, 0.0f, 0.0f),  glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f),
          glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 4.0f, 0.0f),  glm::vec3(0.0f, -8.0f, 0.0f),
      };

      // Initial particle positions
      std::vector<Particle> particleBuffer(static_cast<uint32_t>(attractors.size()) * NUM_PARTICLE);

      std::default_random_engine rndEngine((unsigned)time(nullptr));
      std::normal_distribution<float> rndDist(0.0f, 1.0f);

      for (uint32_t i = 0; i < static_cast<uint32_t>(attractors.size()); i++) {
        for (uint32_t j = 0; j < NUM_PARTICLE; j++) {
          Particle& particle = particleBuffer[i * NUM_PARTICLE + j];

          // First particle in group as heavy center of gravity
          if (j == 0) {
            particle.pos = glm::vec4(attractors[i] * 1.5f, 90000.0f);
            particle.vel = glm::vec4(glm::vec4(0.0f));
          } else {
            // Position
            glm::vec3 position(attractors[i]
                               + glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine)) * 0.75f);
            float len = glm::length(glm::normalize(position - attractors[i]));
            position.y *= 2.0f - (len * len);

            // Velocity
            glm::vec3 angular  = glm::vec3(0.5f, 1.5f, 0.5f) * (((i % 2) == 0) ? 1.0f : -1.0f);
            glm::vec3 velocity = glm::cross((position - attractors[i]), angular)
                                 + glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine) * 0.025f);

            float mass   = (rndDist(rndEngine) * 0.5f + 0.5f) * 75.0f;
            particle.pos = glm::vec4(position, mass);
            particle.vel = glm::vec4(velocity, 0.0f);
          }

          // Color gradient offset
          particle.vel.w = (float)i * 1.0f / static_cast<uint32_t>(attractors.size());
        }
      }

      Buffer<Particle> stagingBuffer(device, particleBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      std::cout << "SingleTimeCommands Copy Buffer start" << std::endl;

      CommandBuffers::SingleTimeCommands(
          device, commandPool, device.graphicsQueue(), [&](const VkCommandBuffer& cmdBuffer) {
            VkBufferCopy copyRegion = {
                .size = this->size(),
            };
            vkCmdCopyBuffer(cmdBuffer, stagingBuffer.buffer(), this->buffer(), 1, &copyRegion);

            const std::optional<uint32_t>& graphicsFamily = device.queueFamilyIndices().graphicsFamily;
            const std::optional<uint32_t>& computeFamily  = device.queueFamilyIndices().computeFamily;

            if (graphicsFamily.value() != computeFamily.value()) {
              const VkBufferMemoryBarrier buffer_barrier = {
                  .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                  .srcAccessMask       = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                  .dstAccessMask       = 0,
                  .srcQueueFamilyIndex = graphicsFamily.value(),
                  .dstQueueFamilyIndex = computeFamily.value(),
                  .buffer              = this->buffer(),
                  .offset              = 0,
                  .size                = this->size(),
              };

              vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                   0, 0, nullptr, 1, &buffer_barrier, 0, nullptr);
            }
          });
      std::cout << "SingleTimeCommands Copy Buffer end" << std::endl;
    }
  };
}