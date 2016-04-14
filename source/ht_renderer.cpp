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

namespace Hatchit {

    namespace Graphics {

        IRenderer*  IRenderer::Instance = nullptr;


        uint32_t IRenderer::GetWidth() const { return m_width; }
        uint32_t IRenderer::GetHeight() const { return m_height; }


        void IRenderer::RegisterRenderPass(RenderPassBaseHandle pass)
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

        void IRenderer::RegisterCamera(Camera camera, uint64_t flags)
        {
            for (int j = 0; flags != 0; j++)
            {
                if (flags & 1)
                {
                    m_renderPassCameras[j].push_back(camera);
                }
                flags >>= 1;
            }
        }
    }
}