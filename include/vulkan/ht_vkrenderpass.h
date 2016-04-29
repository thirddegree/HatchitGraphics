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

#include <ht_renderpass.h>
#include <ht_vulkan.h>
#include <ht_vktexture.h>
#include <ht_vkrendertarget.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKRenderPass : public Core::RefCounted<VKRenderPass>, public RenderPassBase
            {
            public:
                VKRenderPass(Core::Guid ID);
                ~VKRenderPass();

                //Required function for RefCounted classes
                bool Initialize(const std::string& fileName, VKRenderer* renderer);

                //Will this be sent the Objects that it needs to render?
                ///Render the scene
                void VUpdate() override;

                bool VBuildCommandList() override;

                const VkRenderPass& GetVkRenderPass() const;
                const VkCommandBuffer& GetVkCommandBuffer() const;

                const std::vector<IRenderTargetHandle>& GetOutputRenderTargets() const;

            private:
                //Input
                uint32_t m_firstInputTargetSetIndex;
                std::vector<VkDescriptorSet> m_inputTargetDescriptorSets;

                bool setupRenderPass();
                bool setupAttachmentImages();
                bool setupFramebuffer();

                bool allocateCommandBuffer();
                //Mapping set index to maps of binding indicies and render targets
                bool setupDescriptorSets(std::map < uint32_t, std::map < uint32_t, VKRenderTargetHandle >> inputTargets);

                VKRenderer* m_renderer;
                const VkDevice* m_device;
                const VkCommandPool* m_commandPool;
                const VkDescriptorPool* m_descriptorPool;

                VkRenderPass m_renderPass;
                VkCommandBuffer m_commandBuffer;

                //For instance data
                UniformBlock_vk m_instanceBlock;

                std::vector<Image_vk> m_colorImages;
                Image_vk m_depthImage;

                VkFramebuffer m_framebuffer;

            };

            using VKRenderPassHandle = Core::Handle<VKRenderPass>;
        }
    }
}
