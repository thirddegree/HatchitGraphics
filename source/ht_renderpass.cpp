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

        void RenderPassBase::VScheduleRenderRequest(IMaterialHandle material, IMeshHandle mesh)
        {
            RenderRequest renderRequest = {};

            renderRequest.pipeline = material->GetPipeline();
            renderRequest.material = material;
            renderRequest.mesh = mesh;

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

                std::vector<RenderableInstances> instances = m_pipelineList[pipeline];

                //If the pipeline maps to an existing material and mesh, lets increment the count
                bool found = false;
                for (size_t i = 0; i < instances.size(); i++)
                {
                    Renderable renderable = instances[i].renderable;
                    if (renderable.material == material && renderable.mesh == mesh)
                    {
                        instances[i].count++;
                        found = true;
                        break;
                    }
                }

                if(!found)
                    instances.push_back({ material, mesh, 1 });

                m_pipelineList[pipeline] = instances;
            }

            //Done with render requests so we can clear them
            m_renderRequests.clear();
        }
    }
}