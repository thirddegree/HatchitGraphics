/**
**    Hatchit Engine
**    Copyright(c) 2015-2017 Third-Degree
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
 * @file ht_vkimage.cpp
 * @brief VKImage class implementation
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains implementation for VKImage class
 */

#include <ht_vkimage.h>
#include <ht_vkdevice.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKImage::VKImage()
            {
                m_vkDevice = VK_NULL_HANDLE;
                m_vkImage = VK_NULL_HANDLE;
                m_vkImageView = VK_NULL_HANDLE;
                m_vkMemory = VK_NULL_HANDLE;
            }

            VKImage::~VKImage()
            {
                vkDestroyImageView(m_vkDevice, m_vkImageView, nullptr);
                vkDestroyImage(m_vkDevice, m_vkImage, nullptr);
                vkFreeMemory(m_vkDevice, m_vkMemory, nullptr);
            }

            bool VKImage::InitializeImage(VKDevice& device, const VkImageCreateInfo& info)
            {
                m_vkDevice = static_cast<VkDevice>(device);
                
                VkResult err = VK_SUCCESS;

                err = vkCreateImage(m_vkDevice, &info, nullptr, &m_vkImage);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKImage::Initialize(): Failed to create image. %s\n", VKErrorString(err));
                    return false;
                }


                return true;
            }

            bool VKImage::InitializeView(VKDevice& device, VkImageViewCreateInfo& info)
            {
                VkResult err = VK_SUCCESS;

                info.image = m_vkImage;
                err = vkCreateImageView(m_vkDevice, &info, nullptr, &m_vkImageView);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKImage::Initialize(): Failed to create image view. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            bool VKImage::AllocateAndBindMemory(VKDevice& pDevice)
            {
                VkMemoryAllocateInfo memAllocateInfo = {};
                memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocateInfo.pNext = nullptr;
                memAllocateInfo.allocationSize = 0;
                memAllocateInfo.memoryTypeIndex = 0;

                VkMemoryRequirements memoryRequirement = {};
                vkGetImageMemoryRequirements(m_vkDevice, m_vkImage, &memoryRequirement);
                memAllocateInfo.allocationSize = memoryRequirement.size;
                memAllocateInfo.memoryTypeIndex = pDevice.GetMemoryType(memoryRequirement.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                VkResult err = VK_SUCCESS;

                err = vkAllocateMemory(m_vkDevice, &memAllocateInfo, nullptr, &m_vkMemory);

                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKImage::AllocateAndBindMemmory(): Failed to allocate memory. %s\n", VKErrorString(err));
                    return false;
                }

                err = vkBindImageMemory(m_vkDevice, m_vkImage, m_vkMemory, 0);

                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VkImage::AllocateAndBindMemory(): Failed to bind memory to image. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKImage::operator VkImage() const
            {
                return m_vkImage;
            }

            VKImage::operator VkImageView() const
            {
                return m_vkImageView;
            }

            VKImage::operator VkDeviceMemory() const
            {
                return m_vkMemory;
            }

            const VkImage& VKImage::GetImage() const
            {
                return m_vkImage;
            }

            const VkImageView& VKImage::GetImageView() const
            {
                return m_vkImageView;
            }
           
        }
    }
}
