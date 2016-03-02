/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
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
* \class VKSwapchain
* \ingroup HatchitGraphics
*
* \brief A Vulkan based swapchain
*
* This acts as a specialized render pass that is the final pass
* which outputs the the screen.
*/

#pragma once

#include <ht_swapchain.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            struct SwapchainBuffer {
                VkImage image;
                VkCommandBuffer command;
                VkImageView view;
            };

            struct DepthBuffer {
                VkFormat format;
                VkImage image;
                VkMemoryAllocateInfo memAllocInfo;
                VkDeviceMemory memory;
                VkImageView view;
            };

            class VKRenderer;

            class HT_API VKSwapchain : public ISwapchain
            {
            public:
                VKSwapchain(VkInstance* instance, VkDevice* device, VkCommandPool* commandPool);
                ~VKSwapchain();
                
                bool VKPrepare(const VkSurfaceKHR* surface, VkColorSpaceKHR colorSpace, VkPresentModeKHR* presentModes, uint32_t presentModeCount, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D extents);

                bool BuildSwapchain(VkClearValue clearColor);

                VkResult VKGetNextImage(VkSemaphore presentSemaphore);
                VkResult VKPresent(VkQueue queue);

                VkCommandBuffer GetCurrentCommand();

            private:
                VkInstance*      m_instance;
                VkDevice*        m_device;
                VkCommandPool*   m_commandPool;
                
                VkRenderPass m_renderPass;

                VkSwapchainKHR               m_swapchain;
                std::vector<SwapchainBuffer> m_swapchainBuffers;
                std::vector<VkFramebuffer>   m_framebuffers;
                DepthBuffer                  m_depthBuffer;

                void setupFunctionPointers();

                bool prepareSwapchain(VKRenderer* renderer, VkFormat preferredColorFormat, const VkSurfaceKHR* surface, VkColorSpaceKHR colorSpace, VkPresentModeKHR* presentModes, uint32_t presentModeCount, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D surfaceExtents);

                bool prepareSwapchainDepth(VKRenderer* renderer, VkExtent2D extent);

                //Prepare the internal render pass
                bool prepareRenderPass(VkFormat preferredColorFormat);

                //Prepare the framebuffers for the swapchain images
                bool prepareFramebuffers(VkExtent2D extents);

                //Allocate the command buffers
                bool allocateCommandBuffers();

                //Function pointers to swapchain related functionality
                PFN_vkCreateSwapchainKHR
                    fpCreateSwapchainKHR;
                PFN_vkDestroySwapchainKHR
                    fpDestroySwapchainKHR;
                PFN_vkGetSwapchainImagesKHR
                    fpGetSwapchainImagesKHR;
                PFN_vkAcquireNextImageKHR
                    fpAcquireNextImageKHR;
                PFN_vkQueuePresentKHR
                    fpQueuePresentKHR;
            };
        }
    }
}
