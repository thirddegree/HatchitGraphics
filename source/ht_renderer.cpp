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
        
        void IRenderer::AddRenderPass(IRenderPassHandle renderPass) 
        {
            m_renderPasses.push_back(renderPass);
        }

        void IRenderer::RemoveRenderPass(uint32_t index)
        {
            m_renderPasses.erase(m_renderPasses.begin() + index);
        }

        uint32_t IRenderer::GetWidth() const { return m_width; }
        uint32_t IRenderer::GetHeight() const { return m_height; }


        void IRenderer::RegisterCamera(Camera camera, uint32_t flags)
        {
            uint32_t i = flags;
            for (int j = 0; i != 0; j++)
            {
                m_renderPassCameras[j].push_back(camera);
                i <<= 1;
            }
        }
    }
}