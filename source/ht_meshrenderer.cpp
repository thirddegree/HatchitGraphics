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

#include <ht_meshrenderer.h>

namespace Hatchit {

    namespace Graphics {

        MeshRenderer::~MeshRenderer()
        {
        }

        void MeshRenderer::SetMaterial(IMaterialHandle material)
        {
            m_material = material;
            m_pipeline = material->GetPipeline();
        }

        void MeshRenderer::SetRenderPass(IRenderPassHandle renderPass)
        {
            m_renderPass = renderPass;
        }


        void MeshRenderer::SetMesh(IMeshHandle mesh)
        {
            m_mesh = mesh;
        }

        void MeshRenderer::Render()
        {
            m_renderPass->VScheduleRenderRequest(m_pipeline, m_material, m_mesh);
        }
    }
}