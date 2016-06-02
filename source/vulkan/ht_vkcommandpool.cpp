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

#include <ht_vkcommandpool.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKCommandPool::VKCommandPool(VkDevice device) 
            {
                m_device = device;

                m_commandPool = VK_NULL_HANDLE;
            }

            VKCommandPool::~VKCommandPool() 
            {
                //Reset all pool memory
                vkResetCommandPool(m_device, m_commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

                //Delete the pool
                vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            }

            bool VKCommandPool::VInitialize() 
            {
                //Create a command pool for this thread
                VkCommandPoolCreateInfo commandPoolInfo = {};
                commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                commandPoolInfo.pNext = nullptr;
                commandPoolInfo.queueFamilyIndex = 0;
                commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                VkResult err = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandPool::VInitialize(): Failed to create command pool!\n");
                    return false;
                }

                return true;
            }

            VkCommandPool VKCommandPool::GetVKCommandPool() const
            {
                return m_commandPool;
            }
        }
    }
}