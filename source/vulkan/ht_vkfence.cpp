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

#include <ht_vkfence.h>
#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKFence::VKFence()
            {
                m_fence = VK_NULL_HANDLE;
                m_device = VK_NULL_HANDLE;
            }

            VKFence::~VKFence()
            {
                vkDestroyFence(m_device, m_fence, nullptr);
            }

            bool VKFence::Initialize(VKDevice &device, VkFenceCreateFlags flags)
            {
                m_device = device;

                VkResult err = VK_SUCCESS;

                VkFenceCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                info.pNext = nullptr;
                info.flags = flags;

                err = vkCreateFence(device, &info, nullptr, &m_fence);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKFence::Initialize(): Failed to create fence.\n");
                    return false;
                }

                return true;
            }

            bool VKFence::Initialize(VKDevice &device, const VkFenceCreateInfo &info)
            {
                m_device = device;

                VkResult err = VK_SUCCESS;

                err = vkCreateFence(device, &info, nullptr, &m_fence);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKFence::Initialize(): Failed to create fence.\n");
                    return false;
                }

                return true;
            }
        }
    }
}