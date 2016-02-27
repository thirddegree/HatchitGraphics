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

            VKRenderTarget::VKRenderTarget(uint32_t width, uint32_t height,
                VkFormat colorFormat, VkFormat depthFormat) 
            {
                m_width = width;
                m_height = height;

                m_colorFormat = colorFormat;
                m_depthFormat = depthFormat;
            }

            VKRenderTarget::~VKRenderTarget() {} 

            bool VKRenderTarget::VPrepare()
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;
                VkDevice device = renderer->GetVKDevice();

                VkResult err;

                //Color attachment
                VkImageCreateInfo imageInfo = {};
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.pNext = nullptr;
                imageInfo.format = m_colorFormat;
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent.width = m_width;
                imageInfo.extent.height = m_height;
                imageInfo.mipLevels = 1;
                imageInfo.arrayLayers = 1;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                imageInfo.flags = 0;

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

                VkMemoryRequirements memReqs;

                err = vkCreateImage(device, &imageInfo, nullptr, &m_color.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error creating color image!\n");
#endif
                    return false;
                }

                vkGetImageMemoryRequirements(device, m_color.image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, &m_color.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error allocating color image memory!\n");
#endif
                    return false;
                }

                err = vkBindImageMemory(device, m_color.image, m_color.memory, 0);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error binding color image memory!\n");
#endif
                    return false;
                }

                renderer->SetImageLayout(m_color.image, VK_IMAGE_ASPECT_COLOR_BIT,
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

                err = vkCreateImageView(device, &viewInfo, nullptr, &m_color.view);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error creating color image view!\n");
#endif
                    return false;
                }

                //We can reuse the same info structs to build the depth image
                imageInfo.format = m_depthFormat;
                imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

                viewInfo.format = m_depthFormat;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

                err = vkCreateImage(device, &imageInfo, nullptr, &m_depth.image);

                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error creating depth image!\n");
#endif
                    return false;
                }

                vkGetImageMemoryRequirements(device, m_depth.image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, &m_depth.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error allocating depth image memory!\n");
#endif
                    return false;
                }

                err = vkBindImageMemory(device, m_depth.image, m_depth.memory, 0);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error binding depth image memory!\n");
#endif
                    return false;
                }

                renderer->SetImageLayout(m_depth.image, VK_IMAGE_ASPECT_DEPTH_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                err = vkCreateImageView(device, &viewInfo, nullptr, &m_depth.view);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error creating depth image view!\n");
#endif
                    return false;
                }

                //Finally create internal framebuffer
                VkImageView attachments[2];
                attachments[0] = m_color.view;
                attachments[1] = m_depth.view;

                VkFramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.flags = 0;
                framebufferInfo.renderPass = ((VKRenderPass*)m_renderPass)->GetVkRenderPass();
                framebufferInfo.attachmentCount = 2;
                framebufferInfo.pAttachments = attachments;
                framebufferInfo.width = m_width;
                framebufferInfo.height = m_height;
                framebufferInfo.layers = 1;

                err = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffer);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderTarget::VPrepare(): Error creating framebuffer!\n");
#endif
                    return false;
                }

                return true;
            }

            void VKRenderTarget::VReadBind()
            {
            
            }

            void VKRenderTarget::VWriteBind()
            {
            
            }
        }
    }
}
