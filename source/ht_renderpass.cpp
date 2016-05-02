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
#include <ht_renderpass_base.h>
#include <ht_renderer.h>

namespace Hatchit
{
    namespace Graphics
    {            
        RenderPass::RenderPass(Core::Guid ID):
            Core::RefCounted<RenderPass>(std::move(ID))
        {
        }

        bool RenderPass::BuildCommandList() { return true; }

        void RenderPass::SetView(Math::Matrix4 view)
        {
            m_base->VSetView(view);
        }
        void RenderPass::SetProj(Math::Matrix4 proj)
        {
            m_base->VSetProj(proj);
        }

        void RenderPass::ScheduleRenderRequest(MaterialHandle material, MeshHandle mesh, std::vector<Resource::ShaderVariable*> instanceVariables) 
        {
            m_base->ScheduleRenderRequest(material, mesh, instanceVariables);
        }

        uint64_t RenderPass::GetLayerFlags()
        {
            return m_base->GetLayerFlags();
        }

    }
}