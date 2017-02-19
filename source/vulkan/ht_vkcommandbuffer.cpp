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
 * @file ht_vkcommandbuffer.cpp
 * @brief VKCommandBuffer class implementation
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains implementation for VKCommandBuffer class
 */

#include <ht_vkcommandbuffer.h>

namespace Hatchit {
    
    namespace Graphics {

        namespace Vulkan {

            VKCommandBuffer::VKCommandBuffer()
            {
                m_vkDevice = VK_NULL_HANDLE;
                m_vkCommandPool = VK_NULL_HANDLE;
                m_vkCommandBuffer = VK_NULL_HANDLE;
            }

            VKCommandBuffer::~VKCommandBuffer()
            {
                vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, 1, &m_vkCommandBuffer);
            }

            bool VKCommandBuffer::Begin(void) const
            {
                VkResult err = VK_SUCCESS;

                VkCommandBufferBeginInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                info.pNext = nullptr;

                err = vkBeginCommandBuffer(m_vkCommandBuffer, &info);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::Begin(): Failed to begin command buffer recording. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            bool VKCommandBuffer::Begin(const VkCommandBufferBeginInfo* pInfo) const
            {
                if (!pInfo)
                    return false;

                VkResult err = VK_SUCCESS;
                
                err = vkBeginCommandBuffer(m_vkCommandBuffer, pInfo);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::Begin(): Failed to begin command buffer recording. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            bool VKCommandBuffer::End() const
            {
                VkResult err = VK_SUCCESS;

                err = vkEndCommandBuffer(m_vkCommandBuffer);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::End(): Failed to end command buffer recording. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKCommandBuffer::operator VkCommandBuffer() const
            {
                return m_vkCommandBuffer;
            }

            VKCommandBuffer::operator VkCommandBuffer*()
            {
                return &m_vkCommandBuffer;
            }

        }
    }
}