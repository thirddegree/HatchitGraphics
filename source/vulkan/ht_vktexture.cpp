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

            VKTexture::VKTexture(std::string ID) :
                m_device(VKRenderer::RendererInstance->GetVKDevice()),
                Core::RefCounted<VKTexture>(std::move(ID))
            {}

            bool VKTexture::Initialize(const std::string& fileName)
            {
                m_resource = Resource::Texture::GetHandleFromFileName(fileName);
                return true;
                //if (!VBufferImage())
                //    HT_DEBUG_PRINTF("Error creating VKTexture in constructor");
            }

            VKTexture::~VKTexture() 
            {
                vkDestroyImageView(m_device, m_view, nullptr);
                vkDestroyImage(m_device, m_image, nullptr);
                vkFreeMemory(m_device, m_deviceMemory, nullptr);
            }

            VkSampler VKTexture::GetSampler()
            { 
                return m_sampler->GetVkSampler();
            }
            VkImageView VKTexture::GetView() { return m_view; }

            void VKTexture::SetSampler(ISamplerHandle samplerHandle)
            {
                VKSamplerHandle vkSamplerHandle = samplerHandle.DynamicCastHandle<VKSampler>();
                m_sampler = std::move(vkSamplerHandle);
                VBufferImage();
            }

            uint32_t VKTexture::GetWidth() const
            {
                return m_resource->GetWidth();
            }

            uint32_t VKTexture::GetHeight() const
            {
                return m_resource->GetHeight();
            }

            bool VKTexture::VBufferImage()
            {
                VkResult err;

                VKRenderer* renderer = VKRenderer::RendererInstance;

                VkFormat format;
                if (m_resource->GetChannels() == 4)
                {
                    switch (m_sampler->GetColorSpace())
                    {
                    case Resource::Sampler::ColorSpace::GAMMA:
                        format = VK_FORMAT_R8G8B8A8_SRGB;
                        break;
                    case Resource::Sampler::ColorSpace::LINEAR:
                        format = VK_FORMAT_R8G8B8A8_UNORM;
                        break;
                    default:
                        format = VK_FORMAT_R8G8B8A8_UNORM;
                        break;
                    }
                }
                else
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage() Error; cannot process RGB textures; they must be RGBA");
                    return false;
                }

                //Create Image assuming linear tiling!
                VkImageCreateInfo imageCreateInfo = {};
                imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageCreateInfo.pNext = nullptr;
                imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                imageCreateInfo.format = format;
                imageCreateInfo.extent = { m_resource->GetWidth(), m_resource->GetHeight(), 1 };
                imageCreateInfo.mipLevels = m_resource->GetMIPLevels();
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

                //Set the image to be GENERAL before binding so it can map properly
                renderer->SetImageLayout(renderer->GetSetupCommandBuffer(), m_image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

                renderer->FlushSetupCommandBuffer();

                //Get memory requirements
                vkGetImageMemoryRequirements(m_device, m_image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                
                bool success = renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
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
                memcpy(pData, m_resource->GetData(), m_resource->GetWidth() * m_resource->GetHeight() * m_resource->GetChannels());

                vkUnmapMemory(m_device, m_deviceMemory);

                //Set the image to be shader read only
                renderer->CreateSetupCommandBuffer();

                m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                renderer->SetImageLayout(renderer->GetSetupCommandBuffer(), m_image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, m_imageLayout);

                renderer->FlushSetupCommandBuffer();

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
                viewInfo.subresourceRange.levelCount = m_resource->GetMIPLevels();
                viewInfo.image = m_image;
                err = vkCreateImageView(m_device, &viewInfo, nullptr, &m_view);
                assert(!err);

                return true;
            }

        }

    }

}
