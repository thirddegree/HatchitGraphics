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
            }

            VKImage::~VKImage()
            {
                vkDestroyImageView(m_vkDevice, m_vkImageView, nullptr);
                vkDestroyImage(m_vkDevice, m_vkImage, nullptr);
            }

            bool VKImage::Initialize(VKDevice& device, const VkImageCreateInfo* pImageInfo, VkImageViewCreateInfo* pImageViewInfo)
            {
                m_vkDevice = device;

                if (!pImageInfo || !pImageViewInfo)
                    return false;
                
                VkResult err = VK_SUCCESS;

                err = vkCreateImage(device, pImageInfo, nullptr, &m_vkImage);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKImage::Initialize(): Failed to create image. %s\n", VKErrorString(err));
                    return false;
                }

                pImageViewInfo->image = m_vkImage;
                err = vkCreateImageView(device, pImageViewInfo, nullptr, &m_vkImageView);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKImage::Initialize(): Failed to create image view. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKImage::operator VkImage()
            {
                return m_vkImage;
            }

            VKImage::operator VkImageView()
            {
                return m_vkImageView;
            }
        }
    }
}