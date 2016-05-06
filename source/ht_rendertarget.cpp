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

#include <ht_rendertarget.h>
#include <ht_rendertarget_base.h>
#include <ht_gpuresourcepool.h>

namespace Hatchit
{
    namespace Graphics
    {
        RenderTarget::RenderTarget(Core::Guid ID) :
            Core::RefCounted<RenderTarget>(std::move(ID))
        {
            m_base = nullptr;
        }

        RenderTarget::~RenderTarget() 
        {
            delete m_base;
        }

        bool RenderTarget::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the pipeline base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateRenderTarget(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request pipeline immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestRenderTarget(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        Resource::RenderTarget::BlendOp RenderTarget::GetColorBlendOp() const
        {
            return m_base->GetColorBlendOp();
        }
        Resource::RenderTarget::BlendOp RenderTarget::GetAlphaBlendOp() const
        {
            return m_base->GetAlphaBlendOp();
        }

        RenderTargetBase* const RenderTarget::GetBase() const
        {
            return m_base;
        }

    }

}