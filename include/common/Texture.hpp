/**
 * @file Texture.hpp
 * @brief Define Texture class
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <vulkan/vulkan.h>
#include <common/Device.hpp>
#include <common/misc/Device.hpp>
#include <common/buffer/StorageBuffer.hpp>
#include <common/buffer/Buffer.hpp>
#include <common/CommandBuffers.hpp>
#include <common/CommandPool.hpp>

namespace vkl {

    class Texture {

        private:
            const Device& m_device;
            VkImage textureImage;
            VkDeviceMemory textureImageMemory;
            VkImageView textureImageView;
            VkSampler textureSampler;
            const CommandPool& m_commandePool;
            


        public : 
            Texture(const Device &deivce,const CommandPool &commandePool);
            ~Texture();
            void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
            void createTextureImage();
            void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
            void endSingleTimeCommands(VkCommandBuffer commandBuffer);
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

            void createTextureImageView();
            VkImageView createImageView(VkImage image, VkFormat format);
            void createTextureSampler();
    };


}

#endif
