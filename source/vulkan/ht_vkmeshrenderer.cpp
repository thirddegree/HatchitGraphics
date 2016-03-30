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

#include <ht_vkmeshrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKMeshRenderer::VKMeshRenderer()
            {
            }

            VKMeshRenderer::~VKMeshRenderer()
            {
            }

            void VKMeshRenderer::VSetMesh(IMesh* mesh)
            {
                m_mesh = static_cast<VKMesh*>(mesh);
            }

            void VKMeshRenderer::VSetMaterial(IMaterial* material)
            {
                m_material = static_cast<VKMaterial*>(material);
                m_pipeline = static_cast<VKPipeline*>(material->GetPipeline());
            }

            void VKMeshRenderer::VSetRenderPass(IRenderPass* renderPass)
            {
                m_renderPass = static_cast<VKRenderPass*>(renderPass);
            }

            void VKMeshRenderer::VRender()
            {
                m_renderPass->ScheduleRenderRequest(m_pipeline, m_material, m_mesh);
            }
        }
    }
}