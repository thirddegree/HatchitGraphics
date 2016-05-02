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
        void RenderPassBase::ScheduleRenderRequest(MaterialHandle material, MeshHandle mesh, std::vector<Resource::ShaderVariable*> instanceVariables)
        {
            RenderRequest renderRequest = {};

            renderRequest.pipeline = material->GetPipeline();
            renderRequest.material = material;
            renderRequest.mesh = mesh;

            m_renderRequests.push_back(renderRequest);

            //Append instance variables to the array of bytes

            //Determine how much we need to append to the array
            size_t newSize = m_instanceDataSize;
            m_instanceChunkSize = 0;
            for (size_t i = 0; i < instanceVariables.size(); i++)
            {
                m_instanceChunkSize += Resource::ShaderVariable::SizeFromType(instanceVariables[i]->GetType());
                newSize += m_instanceChunkSize;
            }

            //Make an array of the new size and replace the existing one
            BYTE* newArray = new BYTE[newSize];

            if (m_instanceData != nullptr)
            {
                memcpy(newArray, m_instanceData, m_instanceDataSize);
                delete[] m_instanceData;
            }

            m_instanceData = newArray;

            //Copy data to new array
            m_currentInstanceDataOffset = m_instanceDataSize;
            for (size_t i = 0; i < instanceVariables.size(); i++)
            {
                size_t size = Resource::ShaderVariable::SizeFromType(instanceVariables[i]->GetType());
                memcpy(m_instanceData + m_currentInstanceDataOffset, instanceVariables[i]->GetData(), size);
                m_currentInstanceDataOffset += size;
            }

            m_instanceDataSize = m_currentInstanceDataOffset;
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

                PipelineHandle pipeline = renderRequest.pipeline;

                m_pipelineList[pipeline].clear();
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
            }

            //Done with render requests so we can clear them
            m_renderRequests.clear();
        }
    }
}