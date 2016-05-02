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

/**
* \class RenderTarget
* \ingroup HatchitGraphics
*
* \brief An abstraction of a RenderTarget class to be implemented by a graphics language
*/


#pragma once

#include <ht_platform.h>
#include <ht_rendertarget_resource.h>

namespace Hatchit
{
    namespace Graphics 
    {
        class HT_API RenderTargetBase
        {
        public:
            virtual ~RenderTargetBase() { };

            Resource::RenderTarget::BlendOp GetColorBlendOp() const;
            Resource::RenderTarget::BlendOp GetAlphaBlendOp() const;

        protected:
            uint32_t m_width;
            uint32_t m_height;

            Resource::RenderTarget::BlendOp m_colorBlendOp;
            Resource::RenderTarget::BlendOp m_alphaBlendOp;
        };

    }
}