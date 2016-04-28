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

#include <ht_renderer.h>

#ifdef VK_SUPPORT
#include <ht_vkrenderer.h>
#endif

#ifdef DX12_SUPPORT
#include <ht_d3d12renderer.h>
#endif

namespace Hatchit {

    namespace Graphics {

        uint32_t Renderer::GetWidth() const { return m_width; }
        uint32_t Renderer::GetHeight() const { return m_height; }


        void Renderer::RegisterRenderPass(RenderPassBaseHandle pass)
        {
            uint64_t flags = pass->GetLayerFlags();
            for (int j = 0; flags != 0; j++)
            {
                if (flags & 1)
                {
                    m_renderPassLayers[j].push_back(pass);
                }
                flags >>= 1;
            }
        }

        void Renderer::RegisterCamera(Camera camera)
        {
            uint64_t flags = camera.GetLayerFlags();
            for (int j = 0; flags != 0; j++)
            {
                if (flags & 1)
                {
                    m_renderPassCameras[j].push_back(camera);
                }
                flags >>= 1;
            }
        }
        Renderer * Renderer::FromType(RendererType type)
        {
            switch (type)
            {
                case RendererType::VULKAN:
                {
#ifdef VK_SUPPORT
                    return new Vulkan::VKRenderer;
#else
                    return nullptr;
#endif
                } break;
                
                case RendererType::DIRECTX12:
                {
#ifdef DX12_SUPPORT
                    return new DX::D3D12Renderer;
#else
                    return nullptr;
#endif
                } break;
            }
            return nullptr;
        }
    }
}