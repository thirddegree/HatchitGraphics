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
            VKDepthStencil::VKDepthStencil()
            {

            }

            VKDepthStencil::~VKDepthStencil()
            {

            }

            bool VKDepthStencil::Initialize(VKDevice &device, uint32_t width, uint32_t height) {
                m_device = device;

                /**
                 * Query supported depth format
                 */
                VkFormat depthFormat;
                SupportedDepthFormat(device, &depthFormat);

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

                VkMemoryAllocateInfo alloc = {};
                alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc.pNext = nullptr;
                alloc.allocationSize = 0;
                alloc.memoryTypeIndex = 0;

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

                VkMemoryRequirements memRequirements;

                m_buffer.InitializeImage(device, image);
                m_buffer.AllocateAndBindMemory(device);
                m_buffer.InitializeView(device, imageView);


                return true;

            }

            bool VKDepthStencil::SupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *format)
            {
                std::vector<VkFormat> formats = {
                        VK_FORMAT_D32_SFLOAT_S8_UINT,
                        VK_FORMAT_D32_SFLOAT,
                        VK_FORMAT_D24_UNORM_S8_UINT,
                        VK_FORMAT_D16_UNORM_S8_UINT,
                        VK_FORMAT_D16_UNORM
                };


                for(auto& fmt : formats)
                {
                    VkFormatProperties properties;
                    vkGetPhysicalDeviceFormatProperties(physicalDevice, fmt, &properties);

                    /**
                     * Format MUST support depth stencil attachment
                     */
                     if(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                     {
                         *format = fmt;
                         return true;
                     }
                }

                return false;
            }

            VKImage& VKDepthStencil::GetImage()
            {
                return m_buffer;
            }
        }
    }
}