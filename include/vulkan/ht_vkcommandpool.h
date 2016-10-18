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

#pragma once

/**
 * \file ht_vkcommandpool.h
 * \brief VKCommandPool definition
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKCommandPool class
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
            class VKCommandBuffer;

            /**
             * \class VKCommandPool
             * \brief Vulkan command pool wrapper
             *
             * This class wraps functionality associated with VkCommandPool,
             * command buffer generation and management.
             */
            class HT_API VKCommandPool
            {
            public:
                VKCommandPool();

                ~VKCommandPool();

                bool Initialize(VKDevice& device, uint32_t queueFamilyIndex);

                bool Reset(VkCommandPoolResetFlags flags = 0);

                bool AllocateCommandBuffers(VkCommandBufferLevel level, uint32_t count, VKCommandBuffer* pCommandBuffers);

                operator VkCommandPool();
            private:
                VkDevice      m_vkDevice;
                VkCommandPool m_vkCommandPool;

            };
        }
    }
}
