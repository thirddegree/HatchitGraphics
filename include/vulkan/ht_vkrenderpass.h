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
* \class IRenderTarget
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will render the whole scene from a perspective with a graphics language
*
* Used to render a whole scene to an IRenderTexture with a graphics language
* so that it can be used later to complete the final frame.
*/

#pragma once

#include <ht_renderpass.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKRenderPass : public IRenderPass
            {
            public:
                VKRenderPass(VkDevice& device, VkCommandPool& commandPool);
                ~VKRenderPass();

                //Prepare the internal VkRenderPass
                bool VPrepare()   override;

                //Will this be sent the Objects that it needs to render?
                ///Render the scene
                void VUpdate()  override;

                bool VBuildCommandList()    override;

                void VSetClearColor(Color clearColor);

                const VkRenderPass* GetVkRenderPass();
                VkCommandBuffer GetVkCommandBuffer();

            private:

                bool allocateCommandBuffer();

                VkDevice& m_device;
                VkCommandPool& m_commandPool;

                VkRenderPass m_renderPass;
                VkCommandBuffer m_commandBuffer;

                VkClearValue m_clearColor;
            };
        }
    }
}
