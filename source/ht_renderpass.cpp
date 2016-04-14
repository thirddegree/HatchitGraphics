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

#include <ht_renderpass.h>
#include <ht_renderer.h>

namespace Hatchit
{
    namespace Graphics
    {
        bool RenderPassBase::VInitFromResource(const Resource::RenderPassHandle& handle)
        {
            //Initialize from file here
            return true;
        }

        void RenderPassBase::VSetView(Math::Matrix4 view)
        {
            m_view = std::move(view);
        }
        void RenderPassBase::VSetProj(Math::Matrix4 proj)
        {
            m_proj = std::move(proj);
        }

        void RenderPassBase::VScheduleRenderRequest(IPipelineHandle pipeline, IMaterialHandle material, IMeshHandle mesh)
        {
            RenderRequest renderRequest = {};

            renderRequest.pipeline = pipeline;
            renderRequest.material = material;
            renderRequest.mesh = mesh;

            m_view = Math::Matrix4();
            m_proj = Math::Matrix4();

            m_renderRequests.push_back(renderRequest);
        }

        uint64_t RenderPassBase::GetLayerFlags()
        {
            return m_layerflags;
        }

        void RenderPassBase::BuildRenderRequestHeirarchy()
        {
            uint32_t i;

            //Clear past pipeline requests
            for (i = 0; i < m_renderRequests.size(); i++)
            {
                RenderRequest renderRequest = m_renderRequests[i];

                IPipelineHandle pipeline = renderRequest.pipeline;

                m_pipelineList[pipeline].clear();
            }

            //Build new requests
            for (i = 0; i < m_renderRequests.size(); i++)
            {
                RenderRequest renderRequest = m_renderRequests[i];

                IPipelineHandle pipeline = renderRequest.pipeline;
                IMaterialHandle material = renderRequest.material;
                IMeshHandle mesh = renderRequest.mesh;

                m_pipelineList[pipeline].clear();

                m_pipelineList[pipeline].push_back({ material, mesh });
            }
        }
    }
}