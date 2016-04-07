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

/**
* \class VKRenderTarget
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will act as a render target with a given graphics language
*
* Imagine this as a template for an implementation of a class that will
* utilize framebuffer objects with OpenGL or RenderTargets with DirectX
*/

#pragma once

#include <ht_vkrendertarget.h>
#include <ht_vkrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Resource;

            VKRenderTarget::VKRenderTarget(const VkDevice& device, uint32_t width, uint32_t height) :
                m_device(device)
            {
                m_width = width;
                m_height = height;

                VKRenderer* renderer = VKRenderer::RendererInstance;
                m_colorFormat = renderer->GetPreferredImageFormat();
            }
            VKRenderTarget::VKRenderTarget(const std::string& fileName) :
                m_device(VKRenderer::RendererInstance->GetVKDevice()),
                Core::RefCounted<VKRenderTarget>(std::move(fileName)),
                m_resource(RenderTarget::GetHandle(fileName))
                
            {
                m_width = m_resource->GetWidth();
                m_height = m_resource->GetHeight();

                std::string formatString = m_resource->GetFormat();

                //Determine format bit from resource's string
                if (formatString == "BGRA")
                {
                    m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
                }
                else if (formatString == "RGBA")
                {
                    m_colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
                }
                else //If it's not valid or provided; query the renderer for the preferred format
                {
                    VKRenderer* renderer = VKRenderer::RendererInstance;
                    m_colorFormat = renderer->GetPreferredImageFormat();
                }
            }

            VKRenderTarget::~VKRenderTarget() 
            {
                vkFreeMemory(m_device, m_color.memory, nullptr);
                vkDestroyImage(m_device, m_color.image, nullptr);
                vkDestroyImageView(m_device, m_color.view, nullptr);

                vkFreeMemory(m_device, m_depth.memory, nullptr);
                vkDestroyImage(m_device, m_depth.image, nullptr);
                vkDestroyImageView(m_device, m_depth.view, nullptr);

                vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);

                vkFreeMemory(m_device, m_texture.image.memory, nullptr);
                vkDestroyImage(m_device, m_texture.image.image, nullptr);
                vkDestroyImageView(m_device, m_texture.image.view, nullptr);

                vkDestroySampler(m_device, m_texture.sampler, nullptr);
            } 

            bool VKRenderTarget::VPrepare()
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                if (!setupFramebuffer(renderer))
                    return false;

                if (!setupTargetTexture(renderer))
                    return false;

                return true;
            }

            void VKRenderTarget::VReadBind()
            {
            
            }

            void VKRenderTarget::VWriteBind()
            {
            
            }

            bool VKRenderTarget::Blit(VkCommandBuffer buffer)
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Make sure color writes to the render target are finished
                renderer->SetImageLayout(buffer, m_color.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

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

                vkCmdBlitImage(buffer, m_color.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    m_texture.image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

                //Transform textures back
                renderer->SetImageLayout(buffer, m_color.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                return true;
            }

            VkFramebuffer VKRenderTarget::GetVKFramebuffer() { return m_framebuffer; }
            Image VKRenderTarget::GetVKColor() { return m_color; }
            Image VKRenderTarget::GetVKDepth() { return m_depth; }
            Texture& VKRenderTarget::GetVKTexture() { return m_texture; }


            bool VKRenderTarget::setupFramebuffer(VKRenderer* renderer) 
            {
                VkCommandBuffer setupCommand;

                m_depthFormat = renderer->GetPreferredDepthFormat();

                renderer->CreateSetupCommandBuffer();

                setupCommand = renderer->GetSetupCommandBuffer();

                VkResult err;

                //Color attachment
                VkImageCreateInfo imageInfo = {};
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.pNext = nullptr;
                imageInfo.format = m_colorFormat;
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent = { m_width, m_height, 1 };
                imageInfo.mipLevels = 1;
                imageInfo.arrayLayers = 1;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                imageInfo.flags = 0;

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

                VkMemoryRequirements memReqs;

                err = vkCreateImage(m_device, &imageInfo, nullptr, &m_color.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating color image!\n");
                    return false;
                }

                vkGetImageMemoryRequirements(m_device, m_color.image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_color.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error allocating color image memory!\n");
                    return false;
                }

                err = vkBindImageMemory(m_device, m_color.image, m_color.memory, 0);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error binding color image memory!\n");
                    return false;
                }

                renderer->SetImageLayout(setupCommand, m_color.image, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.pNext = nullptr;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = m_colorFormat;
                viewInfo.flags = 0;
                viewInfo.subresourceRange = {};
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;
                viewInfo.image = m_color.image;

                err = vkCreateImageView(m_device, &viewInfo, nullptr, &m_color.view);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating color image view!\n");
                    return false;
                }

                //We can reuse the same info structs to build the depth image
                imageInfo.format = m_depthFormat;
                imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

                err = vkCreateImage(m_device, &imageInfo, nullptr, &(m_depth.image));

                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating depth image!\n");
                    return false;
                }

                viewInfo.format = m_depthFormat;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

                vkGetImageMemoryRequirements(m_device, m_depth.image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_depth.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error allocating depth image memory!\n");
                    return false;
                }

                err = vkBindImageMemory(m_device, m_depth.image, m_depth.memory, 0);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error binding depth image memory!\n");
                    return false;
                }

                renderer->SetImageLayout(setupCommand, m_depth.image,
                    VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

                viewInfo.image = m_depth.image;

                err = vkCreateImageView(m_device, &viewInfo, nullptr, &m_depth.view);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating depth image view!\n");
                    return false;
                }

                renderer->FlushSetupCommandBuffer();

                //Finally create internal framebuffer
                VkImageView attachments[2];
                attachments[0] = m_color.view;
                attachments[1] = m_depth.view;

                VkFramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.flags = 0;
                framebufferInfo.renderPass = *((VKRenderPass*)m_renderPass)->GetVkRenderPass();
                framebufferInfo.attachmentCount = 2;
                framebufferInfo.pAttachments = attachments;
                framebufferInfo.width = m_width;
                framebufferInfo.height = m_height;
                framebufferInfo.layers = 1;

                err = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating framebuffer!\n");
                    return false;
                }

                return true;
            }
            
            bool VKRenderTarget::setupTargetTexture(VKRenderer* renderer) 
            {
                VkResult err;
                VkPhysicalDevice gpu = renderer->GetVKPhysicalDevice();

                //Test that the GPU supports blitting
                VkFormatProperties formatProperties;
                vkGetPhysicalDeviceFormatProperties(gpu, m_colorFormat, &formatProperties);
                assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);
                if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::setupTargetTexture(): GPU does not support blitting!\n");
                    return false;
                }

                renderer->CreateSetupCommandBuffer();

                VkCommandBuffer setupCommandBuffer = renderer->GetSetupCommandBuffer();

                m_texture.width = m_width;
                m_texture.height = m_height;

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

                bool success = renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);
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

                m_texture.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                renderer->SetImageLayout(setupCommandBuffer, m_texture.image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, m_texture.layout);

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

                renderer->FlushSetupCommandBuffer();

                return true;
            }
        }
    }
}
