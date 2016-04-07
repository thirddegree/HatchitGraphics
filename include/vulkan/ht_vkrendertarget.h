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
* \class VKRenderTarget
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will act as a render target with a given graphics language
*
* Imagine this as a template for an implementation of a class that will
* utilize framebuffer objects with OpenGL or RenderTargets with DirectX
*/

#pragma once

#include <ht_rendertarget.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class VKRenderer;

            class HT_API VKRenderTarget : public Core::RefCounted<VKRenderTarget>, public IRenderTarget
            {
            public:
                VKRenderTarget(const VkDevice& device, uint32_t width, uint32_t height);
                VKRenderTarget(std::string ID, const std::string& fileName);
                ~VKRenderTarget();

                ///Prepare the render target with Vulkan
                bool VPrepare()     override;
                ///Override to bind the render target for reading with Vulkan
                void VReadBind()    override;
                ///Override to bind the render target to be written to with Vulkan
                void VWriteBind()   override;

                bool Blit(VkCommandBuffer commandBuffer);

                VkFramebuffer   GetVKFramebuffer();
                Image           GetVKColor();
                Image           GetVKDepth();

                Texture&        GetVKTexture();

            protected:
                const VkDevice& m_device;

                Hatchit::Resource::RenderTargetHandle m_resource;

                VkFormat m_colorFormat;
                VkFormat m_depthFormat;

                Image m_color;
                Image m_depth;
                VkFramebuffer m_framebuffer;

                Texture m_texture;

                bool setupFramebuffer(VKRenderer* renderer);
                bool setupTargetTexture(VKRenderer* renderer);
            };

            using VKRenderTargetHandle = Core::Handle<VKRenderTarget>;
        }
    }
}
