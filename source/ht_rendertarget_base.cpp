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

#include <ht_rendertarget_base.h>       //RenderTarget
#include <ht_rendertarget_resource.h>   //Resource::RenderTarget::BlendOp

namespace Hatchit
{
    namespace Graphics 
    {
        /** Get the blend op to be used on the color channels of this render target
        * \return A BlendOp enum that represents the blending operation to be used on the color channels
        */
        Resource::RenderTarget::BlendOp RenderTargetBase::GetColorBlendOp() const 
        { 
            return m_colorBlendOp; 
        }

        /** Get the blend op to be used on the alpha channel of this render target
        * \return A BlendOp enum that represents the blending operation to be used on the alpha channel
        */
        Resource::RenderTarget::BlendOp RenderTargetBase::GetAlphaBlendOp() const 
        { 
            return m_alphaBlendOp; 
        }
    }
}

