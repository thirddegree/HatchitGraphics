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
 * \file ht_vkswapchain.h
 * \brief VKSwapChain class definition
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 * \author jkvargas (https://github.com/jkvargas)
 *
 * This file contains definition for VKSwapChain class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {

            class VKApplication;
            class VKDevice;

            class HT_API VKSwapChain
            {
                struct Buffer
                {
                    VkImage     image;
                    VkImageView imageView;
                };
            public:
                VKSwapChain();
               
                ~VKSwapChain();

                bool Initialize(const uint32_t width, const uint32_t height, VKApplication& instance, VKDevice& device);

                bool AcquireNextImage(VkSemaphore semaphore, uint32_t* index);
                bool QueuePresent(VkQueue queue, uint32_t index, VkSemaphore semaphore = VK_NULL_HANDLE);

                bool IsValid();

                uint32_t QueueFamilyIndex() const;
                uint32_t GetImageCount() const;
                std::vector<Buffer>& GetBuffers();

            private:
                VkSwapchainKHR m_swapchain;
                VkInstance     m_instance;
                VkDevice       m_device;

                VkSurfaceKHR    m_surface;

                std::vector<Buffer> m_buffers;

                uint32_t m_width;
                uint32_t m_height;
                uint32_t m_queueFamilyIndex;
                uint32_t m_scImgCount;
            };
        }
    }
}
