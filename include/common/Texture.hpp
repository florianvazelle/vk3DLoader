/**
 * @file Texture.hpp
 * @brief Define Texture class
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <common/Device.hpp>
#include <common/VulkanHeader.hpp>
#include <common/buffer/Buffer.hpp>
#include <common/buffer/StorageBuffer.hpp>
#include <common/misc/Device.hpp>
#include <common/CommandPool.hpp>
#include <common/CommandBuffers.hpp>

namespace vkl {

  class Texture {

        private:



        public : 
        
            const Device& m_device;
            VkImage textureImage;
            VkDeviceMemory textureImageMemory;
            VkImageView textureImageView;
            VkSampler textureSampler;
            const CommandPool& m_commandePool;
            

            Texture(const Device &deivce,const CommandPool &commandePool);
            ~Texture();
            void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
            void createTextureImage();
            void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
            void endSingleTimeCommands(VkCommandBuffer commandBuffer);
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

            void createTextureImageView();
            VkImageView createImageView(VkImage image,VkFormat format);
            void createTextureSampler();

  };
}
#endif
