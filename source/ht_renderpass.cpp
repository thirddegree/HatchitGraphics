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
#include <ht_gpuresourcepool.h>
#include <ht_commandpool.h>     //ICommandPool

namespace Hatchit
{
    namespace Graphics
    {            
        RenderPass::RenderPass(Core::Guid ID):
            Core::RefCounted<RenderPass>(std::move(ID))
        {
            m_base = nullptr;
        }

        bool RenderPass::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the pipeline base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateRenderPass(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request pipeline immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestRenderPass(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        bool RenderPass::BuildCommandList(const ICommandPool* commandPool) 
        {
            return m_base->VBuildCommandList(commandPool);
        }

        void RenderPass::SetView(Math::Matrix4 view)
        {
            m_base->SetView(view);
        }
        void RenderPass::SetProj(Math::Matrix4 proj)
        {
            m_base->SetProj(proj);
        }

        void RenderPass::ScheduleRenderRequest(MaterialHandle material, MeshHandle mesh, std::vector<Resource::ShaderVariable*> instanceVariables) 
        {
            m_base->ScheduleRenderRequest(material, mesh, instanceVariables);
        }

        uint64_t RenderPass::GetLayerFlags()
        {
            return m_base->GetLayerFlags();
        }

        RenderPassBase* const RenderPass::GetBase() const
        {
            return m_base;
        }

    }
}