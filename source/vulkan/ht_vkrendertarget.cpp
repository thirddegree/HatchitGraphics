/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
**
**    GNU Lesser General Public License
**    This file may be used under the terms of the GNU Lesser
**    General Public License version 3 as published by the Free
**    Software Foundation and appearing in the file LICENSE.LGPLv3 included
**    in the packaging of this file. Please review the following information
**    to ensure the GNU Lesser General Public License requirements
**    will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

/**
* \class VKRenderTarget
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will act as a render target with a given graphics language
*
* Imagine this as a template for an implementation of a class that will
* utilize framebuffer objects with OpenGL or RenderTargets with DirectX
*/

#include <ht_vkrendertarget.h>
#include <ht_vkswapchain.h>
#include <ht_vktools.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Resource;

            VKRenderTarget::VKRenderTarget()
            {
                m_width = 0;
                m_height = 0;
                m_clearColor = nullptr;
            }

            VKRenderTarget::~VKRenderTarget()
            {
                if (m_clearColor != nullptr)
                    delete m_clearColor;

                vkFreeMemory(m_device, m_texture.image.memory, nullptr);
                vkDestroyImage(m_device, m_texture.image.image, nullptr);
                vkDestroyImageView(m_device, m_texture.image.view, nullptr);

                vkDestroySampler(m_device, m_texture.sampler, nullptr);
            }
            
            bool VKRenderTarget::Initialize(const Resource::RenderTargetHandle& handle, const VkDevice& device, const VkPhysicalDevice& gpu, const VKSwapChain* swapchain)
            {
                m_device = device;
                m_gpu = gpu;

                if (!handle.IsValid())
                {
                    HT_ERROR_PRINTF("VKRenderTarget::Initialize: Handle was null!");
                    return false;
                }

                m_width = handle->GetWidth();
                m_height = handle->GetHeight();

                std::string formatString = handle->GetFormat();

                //Get clear color
                std::vector<float> clearColor = handle->GetClearColor();
                if (clearColor.size() > 0)
                {
                    m_clearColor = new VkClearValue;

                    m_clearColor->color = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};
                }

                //Save blend ops
                m_colorBlendOp = handle->GetColorBlendOp();
                m_alphaBlendOp = handle->GetAlphaBlendOp();

                //Determine format bit from resource's string
                if (formatString == "BGRA")
                {
                    m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
                }
                else if (formatString == "RGBA")
                {
                    m_colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
                }
                else if (formatString == "BGR")
                {
                    m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
                }
                else if (formatString == "RGB")
                {
                    m_colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
                }
                else if (formatString == "R")
                {
                    m_colorFormat = VK_FORMAT_R32_SFLOAT;
                }
                else //If it's not valid or provided; query the renderer for the preferred format
                {
                    m_colorFormat = VKTools::GetPreferredColorFormat();
                }

                if (m_width == 0)
                    m_width = swapchain->GetWidth();
                if (m_height == 0)
                    m_height = swapchain->GetHeight();

                if (!setupTargetTexture())
                    return false;

                return true;
            }

            bool VKRenderTarget::Blit(VkCommandBuffer buffer, const Image_vk& image)
            {
                //Make sure color writes to the render target are finished
                VKTools::SetImageLayout(buffer, image.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

                VKTools::SetImageLayout(buffer, m_texture.image.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

                VkImageBlit blit;

                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = 0;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;

                blit.srcOffsets[0] = { 0,0,0 };
                blit.srcOffsets[1].x = m_width;
                blit.srcOffsets[1].y = m_height;
                blit.srcOffsets[1].z = 1;

                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = 0;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                blit.dstOffsets[0] = { 0,0,0 };
                blit.dstOffsets[1].x = m_texture.width;
                blit.dstOffsets[1].y = m_texture.height;
                blit.dstOffsets[1].z = 1;

                vkCmdBlitImage(buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    m_texture.image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

                //Transform textures back
                VKTools::SetImageLayout(buffer, m_texture.image.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                VKTools::SetImageLayout(buffer, image.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                return true;
            }

            const VkFormat& VKRenderTarget::GetVKColorFormat() const { return m_colorFormat; }
            const Texture_vk& VKRenderTarget::GetVKTexture() const { return m_texture; }

            const uint32_t& VKRenderTarget::GetWidth() const { return m_width; }
            const uint32_t& VKRenderTarget::GetHeight() const { return m_height; }
            const VkClearValue* VKRenderTarget::GetClearColor() const { return m_clearColor; }
            
            bool VKRenderTarget::setupTargetTexture() 
            {
                VkResult err;

                //Test that the GPU supports blitting
                VkFormatProperties formatProperties;
                vkGetPhysicalDeviceFormatProperties(m_gpu, m_colorFormat, &formatProperties);
                assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);
                if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): GPU does not support blitting!\n");
                    return false;
                }

                VKTools::CreateSetupCommandBuffer();

                VkCommandBuffer setupCommandBuffer = VKTools::GetSetupCommandBuffer();

                m_texture.width = m_width;
                m_texture.height = m_height;
                m_texture.mipLevels = 1;

                VkImageCreateInfo imageCreateInfo = {};
                imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageCreateInfo.pNext = nullptr;
                imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                imageCreateInfo.format = m_colorFormat;
                imageCreateInfo.extent = { m_width, m_height, 1 };
                imageCreateInfo.mipLevels = 1;
                imageCreateInfo.arrayLayers = 1;
                imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                imageCreateInfo.flags = 0;

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                
                VkMemoryRequirements memReqs;

                err = vkCreateImage(m_device, &imageCreateInfo, nullptr, &m_texture.image.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): Error creating target texture image!\n");
                    return false;
                }

                vkGetImageMemoryRequirements(m_device, m_texture.image.image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;

                bool success = VKTools::MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);
                assert(success);
                if (!success)
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to find memory properties!\n");
                    return false;
                }


                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_texture.image.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): Error allocating target texture image memory!\n");
                    return false;
                }

                err = vkBindImageMemory(m_device, m_texture.image.image, m_texture.image.memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): Error binding target texture image memory!\n");
                    return false;
                }

                m_texture.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VKTools::SetImageLayout(setupCommandBuffer, m_texture.image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, m_texture.layout);

                //Create a sampler
                VkSamplerCreateInfo samplerInfo = {};
                samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.magFilter = VK_FILTER_LINEAR;
                samplerInfo.minFilter = VK_FILTER_LINEAR;
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                samplerInfo.mipLodBias = 0.0f;
                samplerInfo.maxAnisotropy = 0;
                samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
                samplerInfo.minLod = 0.0f;
                samplerInfo.maxLod = 0.0f;
                samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                
                err = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_texture.sampler);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): Error creating target texture sampler\n");
                    return false;
                }

                //Create image view
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.pNext = nullptr;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = m_colorFormat;
                viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
                viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                viewInfo.image = m_texture.image.image;

                err = vkCreateImageView(m_device, &viewInfo, nullptr, &m_texture.image.view);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): Error creating target texture image view\n");
                    return false;
                }

                VKTools::FlushSetupCommandBuffer();

                return true;
            }
        }
    }
}
