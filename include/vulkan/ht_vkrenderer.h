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

#pragma once

#include <ht_platform.h>
#include <ht_vulkan.h>
#include <ht_renderer.h>
#include <ht_vkrenderpass.h>
#include <ht_vkswapchain.h>
#include <ht_vkmaterial.h>
#include <ht_vkrootlayout.h>
#include <ht_string.h>
#include <vector>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKRenderer : public IRenderer
            {
            public:
                static VKRenderer* RendererInstance;

                VKRenderer();

                ~VKRenderer();

                /** Initialize the renderer
                * \param params The paramaters to intialize this renderer with
                */
                bool VInitialize(const RendererParams& params)  override;

                ///Shutdown the renderer
                void VDeInitialize()    override;

                /** Resizes the the screen
                * \param width The new width of the screen
                * \param height The new height of the screen
                */
                void VResizeBuffers(uint32_t width, uint32_t height)    override;

                /** Sets the color that the screen will clear with
                * \param color The Color to clear the screen with
                */
                void VSetClearColor(const Color& color) override;
                /** Clears the screen with the given clear color
                * \param args Arguments to describe which buffer you want to clear
                */
                void VClearBuffer(ClearArgs args)   override;

                void VRender(float dt)  override;

                ///Present a frame to the screen via a backbuffer
                void VPresent() override;

                /* Get the physical vulkan device
                * \return A physical vulkan device
                */
                VkPhysicalDevice GetVKPhysicalDevice();

                /* Get the core Vulkan device
                * \return The VkDevice
                */
                const VkDevice& GetVKDevice() const;

                /* Get the core Vulkan instance
                * \return The VkInstance for the app
                */
                const VkInstance& GetVKInstance() const;

                /* Get the core Vulkan command pool
                * \return a vulkan command pool
                */
                const VkCommandPool& GetVKCommandPool() const;

                /* Get the core Vulkan descriptor pool
                * \reutrn a vulkan descriptor pool
                */
                const VkDescriptorPool& GetVKDescriptorPool() const;

                const VKRootLayoutHandle& GetVKRootLayoutHandle() const;

                const VkCommandBuffer& GetSetupCommandBuffer() const;

                const VkFormat& GetPreferredImageFormat() const;
                const VkFormat& GetPreferredDepthFormat() const;

                const RendererParams& GetRendererParams() const;

                const VkClearValue& GetClearColor() const;

                void CreateSetupCommandBuffer();
                void FlushSetupCommandBuffer();

                //Reused helpers
                static bool CheckLayers(std::vector<const char*> layerNames, VkLayerProperties* layers, uint32_t layerCount);
                static bool SetImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
                static bool MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex);
                static bool CreateBuffer(VkDevice device, VkBufferUsageFlagBits usage, size_t dataSize, void* data, UniformBlock_vk* uniformBlock);

            private:
                bool m_enableValidation;
                std::vector<const char*>    m_enabledLayerNames;
                std::vector<const char*>    m_enabledExtensionNames;

                RendererParams m_rendererParams; //We will need these later to re-create the swapchain if the window resizes

                //Vuklan data structs
                VkApplicationInfo                       m_appInfo;
                VkInstance                              m_instance;
                VkPhysicalDevice                        m_gpu;
                std::vector<VkQueueFamilyProperties>    m_queueProps;
                VkDevice                                m_device; 
                VkQueue                                 m_queue;
                VkPhysicalDeviceMemoryProperties        m_memoryProps;
                
                VKSwapchain*                            m_swapchain;

                VkCommandBuffer                         m_setupCommandBuffer;

                VkCommandPool                           m_commandPool;
                VkDescriptorPool                        m_descriptorPool;
                
                VkSemaphore                             m_presentSemaphore;
                VkSemaphore                             m_renderSemaphore;
                VkSubmitInfo                            m_submitInfo;

                VkClearValue                            m_clearColor;

                //Resources we want loaded elsewhere
                VKRootLayoutHandle m_rootLayout;
                IMaterialHandle m_material;
                TextureHandle m_texture;
                ISamplerHandle m_sampler;
                IPipelineHandle m_pipeline;
                IMeshHandle m_meshHandle;
                VKRenderPassHandle m_renderPass;

                VKRenderPassHandle m_lightingPass;
                IPipelineHandle m_pointLightingPipeline;
                IMaterialHandle m_pointLightMaterial;
                IMeshHandle m_pointLightMeshHandle;

                float m_angle = 0;

                //Vulkan Callbacks
                PFN_vkCreateDebugReportCallbackEXT m_createDebugReportCallback;
                PFN_vkDestroyDebugReportCallbackEXT m_destroyDebugReportCallback;
                VkDebugReportCallbackEXT msg_callback;
                PFN_vkDebugReportMessageEXT m_debugReportMessage;

                static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                    uint64_t srcObject, size_t location, int32_t msgCode,
                    const char *pLayerPrefix, const char *pMsg, void *pUserData);

                //Core init methods
                bool initVulkan();
                bool initVulkanSwapchain();
                bool prepareVulkan();

                //Helper init methods

                //Helper inits for initVulkan
                bool checkInstanceLayers();
                bool checkInstanceExtensions();
                bool enumeratePhysicalDevices();
                bool checkDeviceLayers();
                bool checkDeviceExtensions();
                bool setupDebugCallbacks();
                bool setupProcAddresses();

                bool setupCommandPool();
                bool setupDescriptorPool();

                bool createDevice();
            };

        }
    }
}