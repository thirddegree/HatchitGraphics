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
            VKSemaphore::VKSemaphore()
            {
                m_semaphore = VK_NULL_HANDLE;
                m_device = VK_NULL_HANDLE;
            }

            VKSemaphore::~VKSemaphore()
            {
                vkDestroySemaphore(m_device, m_semaphore, nullptr);
            }

            bool VKSemaphore::Initialize(VKDevice &device)
            {
                m_device = device;

                VkResult err = VK_SUCCESS;

                VkSemaphoreCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                info.pNext = nullptr;

                err = vkCreateSemaphore(device, &info, nullptr, &m_semaphore);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSemaphore::Initialize(): Failed to create semaphore.\n");
                    return false;
                }

                return true;
            }

            bool VKSemaphore::Initialize(VKDevice &device, const VkSemaphoreCreateInfo &info)
            {
                m_device = device;

                VkResult err = VK_SUCCESS;

                err = vkCreateSemaphore(device, &info, nullptr, &m_semaphore);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSemaphore::Initialize(): Failed to create semaphore.\n");
                    return false;
                }

                return true;
            }
        }
    }
}