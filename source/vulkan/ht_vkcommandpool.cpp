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
 * @file ht_vkcommandpool.cpp
 * @brief VKCommandPool class implementation
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains implementation for VKCommandPool class
 */

#include <ht_vkcommandpool.h>
#include <ht_vkcommandbuffer.h>
#include <ht_vkdevice.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKCommandPool::VKCommandPool()
            {
                m_vkDevice = VK_NULL_HANDLE;
                m_vkCommandPool = VK_NULL_HANDLE;
            }

            VKCommandPool::~VKCommandPool()
            {
                if (m_vkCommandPool != VK_NULL_HANDLE)
                    vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, nullptr);
            }

            VKCommandPool::operator VkCommandPool() const
            {
                return m_vkCommandPool;
            }

            bool VKCommandPool::Initialize(VKDevice &device)
            {
                return Initialize(device, device.GetQueueFamily(VK_QUEUE_GRAPHICS_BIT).graphics);
            }

            bool VKCommandPool::Initialize(VKDevice& device, uint32_t queueFamilyIndex)
            {
                m_vkDevice = static_cast<VkDevice>(device);

                VkResult err = VK_SUCCESS;

                /**
                * Create Vulkan command pool
                */
                VkCommandPoolCreateInfo cInfo = {};
                cInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                cInfo.pNext = nullptr;
                cInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                cInfo.queueFamilyIndex = queueFamilyIndex;

                err = vkCreateCommandPool(m_vkDevice, &cInfo, nullptr, &m_vkCommandPool);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandPool::Initialize(): Failed to create command pool. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            bool VKCommandPool::Reset(VkCommandPoolResetFlags flags) const
            {
                VkResult err = VK_SUCCESS;
                
                /**
                * Attempt to reset the command pool
                */
                err = vkResetCommandPool(m_vkDevice, m_vkCommandPool, flags);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandPool::Reset(): Failed to reset command pool. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            bool VKCommandPool::AllocateCommandBuffers(VkCommandBufferLevel level, uint32_t count, VKCommandBuffer** pCommandBuffers) const
            {
                if (!pCommandBuffers)
                    return false;

                VkResult err = VK_SUCCESS;

                VkCommandBufferAllocateInfo aInfo = {};
                aInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                aInfo.pNext = nullptr;
                aInfo.commandPool = static_cast<VkCommandPool>(*this);
                aInfo.commandBufferCount = count;
                aInfo.level = level;
                
                std::vector<VkCommandBuffer> buffers(count);
                err = vkAllocateCommandBuffers(m_vkDevice, &aInfo, buffers.data());
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandPool::AllocateCommandBuffers(): Failed to allocate command buffer(s) %s\n", VKErrorString(err));
                    return false;
                }

                for (uint32_t i = 0; i < count; i++) {

                    if (!pCommandBuffers[i]) {
                        pCommandBuffers[i] = new VKCommandBuffer;
                        pCommandBuffers[i]->m_vkCommandBuffer = buffers[i];
                        pCommandBuffers[i]->m_vkDevice = m_vkDevice;
                        pCommandBuffers[i]->m_vkCommandPool = static_cast<VkCommandPool>(*this);
                    }
                }

                return true;
            }

        }
    }
}