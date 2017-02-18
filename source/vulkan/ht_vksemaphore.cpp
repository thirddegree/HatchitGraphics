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

#include <ht_vksemaphore.h>
#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKSemaphore::VKSemaphore():
                m_vkDevice(VK_NULL_HANDLE), m_vkSemaphore(VK_NULL_HANDLE)
            {
            }

            VKSemaphore::~VKSemaphore()
            {
                if(m_vkSemaphore != VK_NULL_HANDLE)
                    vkDestroySemaphore(m_vkDevice, m_vkSemaphore, nullptr);
            }

            bool VKSemaphore::Initialize(VKDevice &device)
            {
                m_vkDevice = static_cast<VkDevice>(device);

                VkResult err = VK_SUCCESS;

                VkSemaphoreCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                info.pNext = nullptr;

                err = vkCreateSemaphore(m_vkDevice, &info, nullptr, &m_vkSemaphore);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSemaphore::Initialize(): Failed to create semaphore.\n");
                    return false;
                }

                return true;
            }

            bool VKSemaphore::Initialize(VKDevice &device, const VkSemaphoreCreateInfo &info)
            {
                m_vkDevice = static_cast<VkDevice>(device);

                VkResult err = VK_SUCCESS;

                err = vkCreateSemaphore(m_vkDevice, &info, nullptr, &m_vkSemaphore);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSemaphore::Initialize(): Failed to create semaphore.\n");
                    return false;
                }

                return true;
            }

            VKSemaphore::operator VkSemaphore() const
            {
                return m_vkSemaphore;
            }
        }
    }
}