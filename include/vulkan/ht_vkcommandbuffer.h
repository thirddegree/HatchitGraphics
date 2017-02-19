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

#pragma once

/**
 * @file ht_vkcommandbuffer.h
 * @brief VKCommandBuffer class definition
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKCommandBuffer class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;
            class VKCommandPool;

            class HT_API VKCommandBuffer
            {
            public:
                VKCommandBuffer();

                ~VKCommandBuffer();

                bool Begin(void) const;
                bool Begin(const VkCommandBufferBeginInfo* pInfo) const;
                bool End(void) const;

                explicit operator VkCommandBuffer() const;
                explicit operator VkCommandBuffer*();
            private:
                VkDevice                m_vkDevice;
                VkCommandPool           m_vkCommandPool;
                VkCommandBuffer         m_vkCommandBuffer;

                friend class VKCommandPool;
            };
        }
    }
}