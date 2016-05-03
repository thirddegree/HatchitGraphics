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
#include <ht_vktools.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKTexture::VKTexture()
            {}

            VKTexture::~VKTexture()
            {
                vkDestroyImageView(m_device, m_view, nullptr);
                vkDestroyImage(m_device, m_image, nullptr);
                vkFreeMemory(m_device, m_deviceMemory, nullptr);
            }

            bool VKTexture::Initialize(Resource::TextureHandle handle, const VkDevice& device)
            {
                m_device = device;
                if (!handle.IsValid())
                    return false;

                m_data = handle->GetData();

                m_width = handle->GetWidth();
                m_height = handle->GetHeight();
                m_channels = handle->GetChannels();
                m_mipLevels = handle->GetMIPLevels();

                return VKBufferImage();
            }

            bool VKTexture::Initialize(const VkDevice& device, const BYTE* data, uint32_t width, uint32_t height, uint32_t channelCount, uint32_t mipLevels)
            {
                m_device = device;

                m_data = data;

                m_width = width;
                m_height = height;
                m_channels = channelCount;
                m_mipLevels = mipLevels;

                return VKBufferImage();
            }

            VkImageView VKTexture::GetView() { return m_view; }


            bool VKTexture::VKBufferImage()
            {
                VkResult err;

                VkFormat format;
                if (m_channels == 4 || m_channels == 3)
                {
                    format = VK_FORMAT_R8G8B8A8_UNORM;
                }
                else if (m_channels == 1)
                {
                    format = VK_FORMAT_R32_SFLOAT;
                }
                else
                {
                    HT_DEBUG_PRINTF("VKTexture::VKBufferImage(): Warning: could not determine texture format from channel count; using preferred image format");
                    format = VKTools::GetPreferredColorFormat();

                    //HT_DEBUG_PRINTF("VKTexture::VKBufferImage() Error; could not determine format for texture");
                    //return false;
                }

                //Create Image assuming linear tiling!
                VkImageCreateInfo imageCreateInfo = {};
                imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageCreateInfo.pNext = nullptr;
                imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                imageCreateInfo.format = format;
                imageCreateInfo.extent = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1 };
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
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to create image\n");
                    return false;
                }

                VKTools::CreateSetupCommandBuffer();

                //Set the image to be GENERAL before binding so it can map properly
                VKTools::SetImageLayout(m_image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

                VKTools::FlushSetupCommandBuffer();

                //Get memory requirements
                vkGetImageMemoryRequirements(m_device, m_image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                
                bool success = VKTools::MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(success);
                if (!success)
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to find memory properties!\n");
                    return false;
                }

                //Allocate host memory
                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_deviceMemory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to allocate memory!\n");
                    return false;
                }

                //Bind allocated image
                err = vkBindImageMemory(m_device, m_image, m_deviceMemory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to bind image!\n");
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
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to map memory!\n");
                    return false;
                }

                //Copy image data
                memcpy(pData, m_data, m_width * m_height * m_channels);

                vkUnmapMemory(m_device, m_deviceMemory);

                //Set the image to be shader read only
                VKTools::CreateSetupCommandBuffer();

                m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                VKTools::SetImageLayout(m_image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, m_imageLayout);

                VKTools::FlushSetupCommandBuffer();

                //Setup the image view
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.pNext = nullptr;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = format;
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
