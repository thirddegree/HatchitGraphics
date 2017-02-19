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
 * @file ht_vkswapchain.h
 * @brief VKSwapChain class definition
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 * @author jkvargas (https://github.com/jkvargas)
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

            /**
             * @class VKSwapChain
             * @brief Defines a Vulkan swapchain
             *
             * A Vulkan swapchain that holds the associated image buffers
             * used in screen output
             */
            class HT_API VKSwapChain
            {
                struct Buffer
                {
                    VkImage     image;
                    VkImageView imageView;
                };
            public:

                /**
                 * @brief Constructs swapchain
                 */
                VKSwapChain();

                /**
                 * @brief Destructs swapchain
                 *
                 */
                ~VKSwapChain();

                /**
                 * @brief Initialize swapchain buffers
                 *
                 * @param width Width of swapchain buffers, usually matches surface Window
                 * @param height Height of swapchain buffers, usually matches surface Window
                 * @param instance Vulkan application instance
                 * @param device Vulkan logical device
                 * @return [true] success
                 * @return [false] failure
                 */
                bool Initialize(const uint32_t width, const uint32_t height, VKApplication& instance, VKDevice& device);

                /**
                 * @brief Acquires index of next swapchain buffer
                 * @param semaphore Semaphore object to signal
                 * @param index Index value set with next image
                 * @return [true] : success
                 * @return [false] : failure
                 */
                bool AcquireNextImage(VkSemaphore semaphore, uint32_t* index) const;

                /**
                 * @brief Queues swapchain image for presenting
                 * @param queue Presentable queue to submit
                 * @param index Index of swapchain image to present
                 * @param semaphore Semaphore to signal for synchronization
                 * @return [true] : success
                 * @return [false] : failure
                 */
                bool QueuePresent(VkQueue queue, uint32_t index, VkSemaphore semaphore = VK_NULL_HANDLE) const;

                /**
                 * @brief Checks if valid swapchain handle exists
                 * @return [true] : exists
                 * @return [false] : non-exist
                 */
                bool IsValid() const;


                /**
                 * @brief Gets format of swapchain surface
                 * @return surface format
                 */
                VkFormat GetSurfaceFormat() const;

                /**
                 * @brief Gets number of swapchain buffers
                 * @return swapchain buffer count
                 */
                uint32_t GetBufferCount() const;

                /**
                 * @brief Gets collection of swapchain buffers
                 * @return swapchain buffers
                 */
                std::vector<Buffer>& GetBuffers();

            private:
                VkSwapchainKHR      m_vkSwapChain;;
                VkInstance          m_vkInstance;
                VkDevice            m_vkDevice;
                VkPhysicalDevice    m_vkPhysicalDevice;
                VkSurfaceKHR        m_vkSurface;

                std::vector<Buffer> m_buffers;

                uint32_t m_width;
                uint32_t m_height;
                uint32_t m_bufferCount;
                VkFormat m_surfaceFormat;
            };
        }
    }
}
