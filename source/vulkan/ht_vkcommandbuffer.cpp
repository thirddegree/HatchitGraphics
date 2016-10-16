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

            VKCommandBuffer::VKCommandBuffer(VkCommandBufferLevel level)
            {
                m_vkCommandBufferLevel = level;
                m_vkCommandBuffer = VK_NULL_HANDLE;
            }

            VKCommandBuffer::~VKCommandBuffer()
            {

            }

            bool VKCommandBuffer::Initialize(VKDevice& device)
            {
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