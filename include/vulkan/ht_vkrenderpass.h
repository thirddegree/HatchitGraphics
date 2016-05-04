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
* \class VKRenderPass
* \ingroup HatchitGraphics
*
* \brief A Render Pass implemented with Vulkan
*
* This render pass uses Vulkan to create a command buffer fit for submission to the renderer.
* It can be a part of a RenderLayer and takes in Render Submissions made up of Materials and Meshes.
*/

#pragma once

#include <ht_renderpass_base.h>
#include <ht_vulkan.h>
#include <ht_vktexture.h>
#include <ht_vkrendertarget.h>
#include <ht_vkrootlayout.h>
#include <ht_rootlayout.h>      //RootLayoutHandle

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKRenderPass : public RenderPassBase
            {
            public:
                VKRenderPass();
                ~VKRenderPass();

                //Required function for RefCounted classes
                bool Initialize(const Resource::RenderPassHandle& handle, const VkDevice& device,
                    const VkCommandPool& commandPool, const VkDescriptorPool& descriptorPool, const VKSwapChain* swapchain);

                //Will this be sent the Objects that it needs to render?
                ///Render the scene
                void VUpdate() override;

                bool VBuildCommandList() override;

                const VkRenderPass& GetVkRenderPass() const;
                const VkCommandBuffer& GetVkCommandBuffer() const;
                const VKRootLayout* GetVKRootLayout() const;

                const std::vector<RenderTargetHandle>& GetOutputRenderTargets() const;

            private:
                //Input
                uint32_t m_firstInputTargetSetIndex;
                std::vector<VkDescriptorSet> m_inputTargetDescriptorSets;

                const VKSwapChain* m_swapchain;

                bool setupRenderPass();
                bool setupAttachmentImages();
                bool setupFramebuffer();

                bool allocateCommandBuffer();
                //Mapping set index to maps of binding indicies and render targets
                bool setupDescriptorSets(std::map < uint32_t, std::map < uint32_t, VKRenderTarget* >> inputTargets);

                VkDevice m_device;
                VkCommandPool m_commandPool;
                VkDescriptorPool m_descriptorPool;

                VkRenderPass m_renderPass;
                VkCommandBuffer m_commandBuffer;
                
                Graphics::RootLayoutHandle m_rootLayoutHandle; //To keep this referenced
                VKRootLayout* m_rootLayout;
                
                //For instance data
                UniformBlock_vk m_instanceBlock;

                std::vector<Image_vk> m_colorImages;
                Image_vk m_depthImage;

                VkFramebuffer m_framebuffer;
            };
        }
    }
}
