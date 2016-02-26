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

#include <ht_vkrenderpass.h>
#include <ht_vkrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKRenderPass::VKRenderPass() {}
            VKRenderPass::~VKRenderPass() {}

            //Will this be sent the Objects that it needs to render?
            ///Render the scene
            void VKRenderPass::VRender()
            {
            }

            void VKRenderPass::VSetRenderTarget(IRenderTarget* renderTarget)
            {
                m_renderTarget = renderTarget;
            }

            void VKRenderPass::VPrepare() 
            {
                //TODO: Construct command buffer

                VKRenderer::RendererInstance->AddRenderPass(this);
            }

            VkCommandBuffer VKRenderPass::GetVkCommandBuffer() { return m_commandBuffer; }

            VkRenderPass VKRenderPass::GetVkRenderPass() { return m_renderPass; }
        }
    }
}
