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

                bool Initialize(VKApplication& instance, VKDevice& device);

                bool IsValid();

                uint32_t QueueFamilyIndex() const;

            private:
                VkSwapchainKHR m_swapchain;
                VkInstance     m_instance;
                VkDevice       m_device;

                VkSurfaceKHR    m_surface;

                std::vector<Buffer> m_buffers;

                uint32_t m_width;
                uint32_t m_height;
                uint32_t m_queueFamilyIndex;
            };
        }
    }
}
