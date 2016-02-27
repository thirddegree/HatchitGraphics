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

            m_renderRequests.push_back(renderRequest);
        }

        void IRenderPass::SetWidth(float width) { m_width = width; }
        void IRenderPass::SetHeight(float height) { m_height = height; }
    }
}