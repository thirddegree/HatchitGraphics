/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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

#include <ht_rendertarget_base.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class VKSwapChain;

            class HT_API VKRenderTarget : public RenderTargetBase
            {
            public:
                VKRenderTarget();
                ~VKRenderTarget();

                //Required function from RefCounted classes
                bool Initialize(const Resource::RenderTargetHandle& handle, const VkDevice& device, const VkPhysicalDevice& gpu, const VKSwapChain* swapchain);

                bool Blit(VkCommandBuffer commandBuffer, const Image_vk& image);

                const VkFormat&     GetVKColorFormat() const;
                const Texture_vk&   GetVKTexture() const;

                const uint32_t& GetWidth() const;
                const uint32_t& GetHeight() const;
                const VkClearValue* GetClearColor() const;

            protected:
                VkDevice m_device;
                VkPhysicalDevice m_gpu;

                VkClearValue* m_clearColor;

                VkFormat m_colorFormat;
                Texture_vk m_texture;

                bool setupTargetTexture();
            };
        }
    }
}
