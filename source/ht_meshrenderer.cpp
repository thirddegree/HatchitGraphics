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
#include <ht_pipeline.h>

namespace Hatchit {

    namespace Graphics {

        MeshRenderer::MeshRenderer(Renderer* renderer)
        {
            m_renderer = renderer;
        }

        MeshRenderer::~MeshRenderer()
        {
        }

        void MeshRenderer::SetMaterial(MaterialHandle material)
        {
            m_material = material;
            m_pipeline = material->GetPipeline();
            m_renderPass = material->GetRenderPasses()[0];
        }

        void MeshRenderer::SetMesh(MeshHandle mesh)
        {
            m_mesh = mesh;
        }

        void MeshRenderer::SetInstanceData(ShaderVariableChunk* data)
        {
            m_instanceData = data;
        }

        void MeshRenderer::Render()
        {          
            m_renderer->RegisterRenderRequest(m_renderPass, m_material, m_mesh, m_instanceData);
        }
    }
}