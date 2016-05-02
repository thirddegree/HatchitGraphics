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

namespace Hatchit
{
    namespace Graphics
    {
        RenderTarget::RenderTarget(Core::Guid ID) :
            Core::RefCounted<RenderTarget>(std::move(ID))
        {
            m_base = nullptr;
        }

        Resource::RenderTarget::BlendOp RenderTarget::GetColorBlendOp() const
        {
            return m_base->VGetColorBlendOp();
        }
        Resource::RenderTarget::BlendOp RenderTarget::GetAlphaBlendOp() const
        {
            return m_base->VGetAlphaBlendOp();
        }

    }

}