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

namespace Hatchit {

    namespace Graphics {

        void IRenderPass::ScheduleRenderRequest(IPipeline* pipeline, IMaterial* material, IMesh* mesh)
        {
            RenderRequest renderRequest = {};

            renderRequest.pipeline  = pipeline;
            renderRequest.material  = material;
            renderRequest.mesh      = mesh;

            m_view = Math::Matrix4();
            m_proj = Math::Matrix4();

            m_renderRequests.push_back(renderRequest);
        }

        void IRenderPass::SetRenderTarget(IRenderTarget* renderTarget)
        {
            m_renderTarget = renderTarget;
        }

        void IRenderPass::SetWidth(uint32_t width) { m_width = width; }
        void IRenderPass::SetHeight(uint32_t height) { m_height = height; }

        void IRenderPass::SetView(Math::Matrix4 view) 
        {
            m_view = view; 
        }
        void IRenderPass::SetProj(Math::Matrix4 proj) 
        { 
            m_proj = proj; 
        }

        void IRenderPass::BuildRenderRequestHeirarchy() 
        {
            uint32_t i;

            //Clear past pipeline requests
            for (i = 0; i < m_renderRequests.size(); i++)
            {
                RenderRequest renderRequest = m_renderRequests[i];

                IPipeline* pipeline = renderRequest.pipeline;

                m_pipelineList[pipeline].clear();
            }

            //Build new requests
            for (i = 0; i < m_renderRequests.size(); i++)
            {
                RenderRequest renderRequest = m_renderRequests[i];

                IPipeline* pipeline = renderRequest.pipeline;
                IMaterial* material = renderRequest.material;
                IMesh* mesh = renderRequest.mesh;

                m_pipelineList[pipeline].clear();

                m_pipelineList[pipeline].push_back({material, mesh});
            }
        }
    }
}