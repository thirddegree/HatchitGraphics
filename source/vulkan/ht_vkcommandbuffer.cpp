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

#include <ht_vkcommandbuffer.h>
#include <ht_vkdevice.h>

namespace Hatchit {
    
    namespace Graphics {

        namespace Vulkan {

            VKCommandBuffer::VKCommandBuffer()
            {
                m_vkDevice = VK_NULL_HANDLE;
                m_vkCommandPool = VK_NULL_HANDLE;
                m_vkCommandBuffer = VK_NULL_HANDLE;
                m_isBegin = false;
            }

            VKCommandBuffer::~VKCommandBuffer()
            {
                vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, 1, &m_vkCommandBuffer);
            }

            bool VKCommandBuffer::Begin(const VkCommandBufferBeginInfo* pInfo)
            {
                if (!pInfo)
                    return false;

                if (m_isBegin)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::Begin(): Command buffer must not already be recording.\n");
                    return false;
                }

                VkResult err = VK_SUCCESS;
                
                err = vkBeginCommandBuffer(m_vkCommandBuffer, pInfo);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::Begin(): Failed to begin command buffer recording. %s\n", VKErrorString(err));
                    return false;
                }

                m_isBegin = true;
                return true;
            }

            bool VKCommandBuffer::End()
            {
                if (!m_isBegin)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::End(): Command buffer must be recording.\n");
                    return false;
                }

                VkResult err = VK_SUCCESS;

                err = vkEndCommandBuffer(m_vkCommandBuffer);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKCommandBuffer::End(): Failed to end command buffer recording. %s\n", VKErrorString(err));
                    return false;
                }
                m_isBegin = false;

                return true;
            }

            VKCommandBuffer::operator VkCommandBuffer()
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