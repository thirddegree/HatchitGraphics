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

#include <ht_renderpass_base.h>

namespace Hatchit 
{
    namespace Graphics 
    {
        void RenderPassBase::SetView(Math::Matrix4 view) { m_view = view; }
        void RenderPassBase::SetProj(Math::Matrix4 proj) { m_proj = proj; }

        void RenderPassBase::ScheduleRenderRequest(MaterialHandle material, MeshHandle mesh, ShaderVariableChunk* instanceVariables)
        {
            RenderRequest renderRequest = {};

            renderRequest.pipeline = material->GetPipeline();
            renderRequest.material = material;
            renderRequest.mesh = mesh;
            renderRequest.instanceData = instanceVariables;

            m_renderRequests.push_back(renderRequest);
        }

        uint64_t RenderPassBase::GetLayerFlags()
        {
            return m_layerflags;
        }

        void RenderPassBase::BuildRenderRequestHeirarchy()
        {
            uint32_t i;

            //Clear past request data
            for (i = 0; i < m_renderRequests.size(); i++)
            {
                RenderRequest renderRequest = m_renderRequests[i];

                MeshHandle mesh = renderRequest.mesh;
                PipelineHandle pipeline = renderRequest.pipeline;

                m_pipelineList[pipeline].clear();
                m_instanceData[mesh].clear();
            }

            //Build new requests
            for (i = 0; i < m_renderRequests.size(); i++)
            {
                RenderRequest renderRequest = m_renderRequests[i];

                PipelineHandle pipeline = renderRequest.pipeline;
                MaterialHandle material = renderRequest.material;
                MeshHandle mesh = renderRequest.mesh;

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

                if (!found)
                    instances.push_back({ material, mesh, 1 });

                m_pipelineList[pipeline] = instances;

                //Take the instance data for this mesh and add it to the map of instance data
                //This is to make sure that different meshes will render in chunks with their own separate instance data
                ShaderVariableChunk* instanceData = renderRequest.instanceData;
                m_instanceData[mesh].push_back(instanceData);
            }

            //Done with render requests so we can clear them
            m_renderRequests.clear();
        }
    }
}