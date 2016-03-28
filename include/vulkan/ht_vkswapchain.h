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

#include <ht_vkpipeline.h>  
#include <ht_vkrendertarget.h>

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
                VKSwapchain(VkInstance& instance, VkPhysicalDevice& gpu, VkDevice& device, VkCommandPool& commandPool);
                ~VKSwapchain();
                
                bool VKPrepare(VkSurfaceKHR surface, VkColorSpaceKHR colorSpace);
                bool VKPrepareResources();

                bool BuildSwapchain(VkClearValue clearColor);

                VkResult VKGetNextImage(VkSemaphore presentSemaphore);
                bool VKPostPresentBarrier(const VkQueue& queue);
                bool VKPrePresentBarrier(const VkQueue& queue);
                VkResult VKPresent(const VkQueue& queue, const VkSemaphore& renderSemaphore);

                VkCommandBuffer GetCurrentCommand();

            private:
                VkInstance&         m_instance;
                VkPhysicalDevice&   m_gpu;
                VkDevice&           m_device;
                VkCommandPool&      m_commandPool;

                VkRenderPass            m_renderPass;
                VKPipeline*             m_pipeline;
                VkDescriptorSetLayout   m_descriptorSetLayout;
                VkDescriptorSet         m_descriptorSet;

                std::vector<VkCommandBuffer> m_postPresentCommands;
                std::vector<VkCommandBuffer> m_prePresentCommands;

                VkSwapchainKHR               m_swapchain;
                std::vector<SwapchainBuffer> m_swapchainBuffers;
                std::vector<VkFramebuffer>   m_framebuffers;
                DepthBuffer                  m_depthBuffer;

                UniformBlock    m_vertexBuffer;

                //Prepare the swapchain base
                bool prepareSwapchain(VKRenderer* renderer, VkSurfaceKHR surface, VkFormat preferredColorFormat, VkColorSpaceKHR colorSpace,
                    std::vector<VkPresentModeKHR> presentModes, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D surfaceExtents);

                //Prepare the swapchain depth buffer
                bool prepareSwapchainDepth(VKRenderer* renderer, VkExtent2D extent);

                //Prepare the internal render pass
                bool prepareRenderPass(VkFormat preferredColorFormat);

                //Prepare the framebuffers for the swapchain images
                bool prepareFramebuffers(VkExtent2D extents);

                //Allocate the command buffers
                bool allocateCommandBuffers();

                bool submitBarrier(const VkQueue& queue, const VkCommandBuffer& command);
            };
        }
    }
}
