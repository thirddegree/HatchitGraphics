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

#include <ht_vkdepthstencil.h>
#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKDepthStencil::VKDepthStencil(): 
                m_vkDevice(VK_NULL_HANDLE),
                m_buffer(std::make_unique<VKImage>())
            {
            }

            VKDepthStencil::~VKDepthStencil()
            {
                
            }

            bool VKDepthStencil::Initialize(VKDevice &device, uint32_t width, uint32_t height) 
            {
                m_vkDevice = static_cast<VkDevice>(device);

                /**
                 * Query supported depth format
                 */
                VkFormat depthFormat;
                device.GetSupportedDepthFormat(depthFormat);

                /**
                 * Setup image info
                 */
                VkImageCreateInfo image = {};
                image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                image.pNext = nullptr;
                image.imageType = VK_IMAGE_TYPE_2D;
                image.format = depthFormat;
                image.extent = { width, height, 1};
                image.mipLevels = 1;
                image.arrayLayers = 1;
                image.samples = VK_SAMPLE_COUNT_1_BIT;
                image.tiling = VK_IMAGE_TILING_OPTIMAL;
                image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                image.flags = 0;

                VkImageViewCreateInfo imageView = {};
                imageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageView.pNext = nullptr;
                imageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
                imageView.format = depthFormat;
                imageView.flags = 0;
                imageView.subresourceRange = {};
                imageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                imageView.subresourceRange.baseMipLevel = 0;
                imageView.subresourceRange.levelCount = 1;
                imageView.subresourceRange.baseArrayLayer = 0;
                imageView.subresourceRange.layerCount = 1;



                if (!m_buffer->InitializeImage(device, image))
                {
                    HT_ERROR_PRINTF("Failed to initialize depth stencil image.\n");
                    return false;
                }

                if (!m_buffer->AllocateAndBindMemory(device))
                {
                    HT_ERROR_PRINTF("Failed to allocate/bind memory for depth stencil.\n");
                    return false;
                }

                if (!m_buffer->InitializeView(device, imageView))
                {
                    HT_ERROR_PRINTF("Failed to initialize depth stencil view.\n");
                    return false;
                }

                return true;
            }

            VKImage& VKDepthStencil::GetImage() const
            {
                return *m_buffer;
            }
        }
    }
}