/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
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

#include <ht_vktexture.h>
#include <ht_vkrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKTexture::VKTexture(VkDevice device, VkFormat format) 
            {
                m_device = device;
                m_format = format;
            }
            VKTexture::~VKTexture() 
            {
                vkDestroyImageView(m_device, m_view, nullptr);
                vkDestroyImage(m_device, m_image, nullptr);
                vkDestroySampler(m_device, m_sampler, nullptr);
                vkFreeMemory(m_device, m_deviceMemory, nullptr);
            }

            bool VKTexture::VBufferImage()
            {
                VkResult err;

                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Create Image assuming optimal tiling!
                VkImageCreateInfo imageCreateInfo = {};
                imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageCreateInfo.pNext = nullptr;
                imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                imageCreateInfo.format = m_format;
                imageCreateInfo.extent = { m_width, m_height, 1 };
                imageCreateInfo.mipLevels = m_mipLevels;
                imageCreateInfo.arrayLayers = 1;
                imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
                imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
                imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                imageCreateInfo.flags = 0;
                
                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                
                VkMemoryRequirements memReqs;

                //Create Image
                err = vkCreateImage(m_device, &imageCreateInfo, nullptr, &m_image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKTexture::VBufferImage(): Failed to create image\n");
#endif
                    return false;
                }

                //Get memory requirements
                vkGetImageMemoryRequirements(m_device, m_image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                
                bool success = renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(success);
                if (!success)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKTexture::VBufferImage(): Failed to find memory properties!\n");
#endif
                    return false;
                }

                //Allocate host memory
                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_deviceMemory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKTexture::VBufferImage(): Failed to allocate memory!\n");
#endif
                    return false;
                }

                //Bind allocated image
                err = vkBindImageMemory(m_device, m_image, m_deviceMemory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKTexture::VBufferImage(): Failed to bind image!\n");
#endif
                    return false;
                }

                VkImageSubresource subRes = {};
                subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                VkSubresourceLayout subResLayout;
                void* pData;

                vkGetImageSubresourceLayout(m_device, m_image, &subRes, &subResLayout);
                
                //Map memory
                err = vkMapMemory(m_device, m_deviceMemory, 0, memReqs.size, 0, &pData);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKTexture::VBufferImage(): Failed to map memory!\n");
#endif
                    return false;
                }

                //Copy image data
                memcpy(pData, m_data, m_width * m_height * m_channels);

                vkUnmapMemory(m_device, m_deviceMemory);

                //Set the image to be shader read only
                m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                renderer->SetImageLayout(renderer->GetSetupCommandBuffer(), m_image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, m_imageLayout);

                //Setup the sampler
                VkSamplerCreateInfo samplerInfo = {};
                samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.pNext = nullptr;
                samplerInfo.magFilter = VK_FILTER_LINEAR;
                samplerInfo.minFilter = VK_FILTER_LINEAR;
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                samplerInfo.mipLodBias = 0.0f;
                samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
                samplerInfo.minLod = 0.0f;
                samplerInfo.maxLod = static_cast<float>(m_mipLevels);
                samplerInfo.maxAnisotropy = 8;
                samplerInfo.anisotropyEnable = VK_TRUE;
                samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

                err = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKTexture::VBufferImage(): Failed to create sampler!\n");
#endif
                    return false;
                }

                //Setup the image view
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.pNext = nullptr;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = m_format;
                viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;
                viewInfo.subresourceRange.levelCount = m_mipLevels;
                viewInfo.image = m_image;
                err = vkCreateImageView(m_device, &viewInfo, nullptr, &m_view);
                assert(!err);

                return true;
            }

        }

    }

}