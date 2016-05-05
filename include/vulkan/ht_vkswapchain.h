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
* \class VKSwapChain
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
#include <ht_vkqueue.h>

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

            class HT_API VKSwapChain : public SwapChain
            {
            public:
                VKSwapChain(const RendererParams& rendererParams, VKDevice* device, VKQueue* queue);
                ~VKSwapChain();
                
                void VClear(float* color)                           override;
                bool VInitialize(uint32_t width, uint32_t height)   override;
                void VResize(uint32_t width, uint32_t height)       override;
                void VPresent()                                     override;

                const VkCommandBuffer&  GetVKCurrentCommand() const;
                const VkSurfaceKHR&     GetVKSurface() const;
                const uint32_t&         GetVKGraphicsQueueIndex() const;

                const VkClearValue&     GetVKClearColor() const;

                bool BuildSwapchainCommands(VkClearValue clearColor);

                VkResult VKGetNextImage(VkSemaphore presentSemaphore);
                bool VKPostPresentBarrier(const VkQueue& queue);
                bool VKPrePresentBarrier(const VkQueue& queue);
                VkResult VKPresent(const VkQueue& queue, const VkSemaphore& renderSemaphore);

                void VKSetIncomingRenderPass(VKRenderPass* renderPass);

            private:
                VkInstance          m_instance;
                VkPhysicalDevice    m_gpu;
                VkDevice            m_device;
                VkQueue             m_queue;
                VkCommandPool       m_commandPool;
                VkDescriptorPool    m_descriptorPool;

                VkSemaphore     m_presentSemaphore;
                VkSemaphore     m_renderSemaphore;
                VkSubmitInfo    m_submitInfo;

                bool m_dirty;

                VkSurfaceKHR                            m_surface;
                VkPhysicalDeviceProperties              m_gpuProps;
                std::vector<VkQueueFamilyProperties>    m_queueProps;
                uint32_t                                m_graphicsQueueNodeIndex;

                VkFormat        m_preferredColorFormat;
                VkFormat        m_preferredDepthFormat;
                VkColorSpaceKHR m_colorSpace;
                VkClearValue    m_clearColor;

                //Void pointers because their contents are dependant on the platform
                void* m_window;
                void* m_display;

                VkRenderPass            m_renderPass;
                PipelineHandle          m_pipelineHandle; //Hold this so it doesn't deref and destroy the pipeline
                VKPipeline*             m_pipeline;
                VkDescriptorSet         m_descriptorSet;

                std::vector<VkCommandBuffer> m_postPresentCommands;
                std::vector<VkCommandBuffer> m_prePresentCommands;

                VkSwapchainKHR               m_swapchain;
                std::vector<SwapchainBuffer> m_swapchainBuffers;
                std::vector<VkFramebuffer>   m_framebuffers;
                DepthBuffer                  m_depthBuffer;

                UniformBlock_vk         m_vertexBuffer;
                std::vector<Texture_vk> m_inputTextures;

                bool vkPrepare();
                bool vkPrepareResources();

                bool createAllocatorPools();

                bool prepareSurface();

                bool getQueueProperties();

                bool findSutibleQueue();

                bool getPreferredFormats();

                //Prepare the swapchain base
                bool prepareSwapchain(VkFormat preferredColorFormat, VkColorSpaceKHR colorSpace,
                    std::vector<VkPresentModeKHR> presentModes, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D surfaceExtents);

                //Prepare the swapchain depth buffer
                bool prepareSwapchainDepth(const VkFormat& preferredDepthFormat, VkExtent2D extent);

                //Prepare the internal render pass
                bool prepareRenderPass();

                //Prepare the framebuffers for the swapchain images
                bool prepareFramebuffers(VkExtent2D extents);

                //Allocate the command buffers
                bool allocateCommandBuffers();

                bool submitBarrier(const VkQueue& queue, const VkCommandBuffer& command);

                //Helpers for destruction / recreation
                void destroySurface();
                void destroyPipeline();
                void destroyDepth();
                void destroyFramebuffers();
                void destroySwapchainBuffers();
                void destroyRenderPass();
                void destroySwapchain();
                
            };
        }
    }
}
