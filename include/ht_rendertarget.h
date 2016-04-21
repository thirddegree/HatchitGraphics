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
* \class IRenderTarget
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will act as a render target with a given graphics language
*
* Imagine this as a template for an implementation of a class that will
* utilize framebuffer objects with OpenGL or RenderTargets with DirectX
*/

#pragma once

#include <ht_platform.h>
#include <ht_math.h>
#include <ht_rendertarget_resource.h>

namespace Hatchit {

    namespace Graphics {
        
        class IRenderPass;

        class HT_API IRenderTarget
        {
        public:
            virtual ~IRenderTarget() { };

            ///Prepare the render target with a graphics language
            virtual bool VPrepare() = 0;

            virtual Resource::RenderTarget::BlendOp GetColorBlendOp() const = 0;
            virtual Resource::RenderTarget::BlendOp GetAlphaBlendOp() const = 0;
        };

        class HT_API RenderTargetBase : public IRenderTarget
        {
        public:
            virtual ~RenderTargetBase() { };

            Resource::RenderTarget::BlendOp GetColorBlendOp() const override;
            Resource::RenderTarget::BlendOp GetAlphaBlendOp() const override;

        protected:
            uint32_t m_width;
            uint32_t m_height;

            Resource::RenderTarget::BlendOp m_colorBlendOp;
            Resource::RenderTarget::BlendOp m_alphaBlendOp;
        };

        using IRenderTargetHandle = Core::Handle<IRenderTarget>;
        using RenderTargetBaseHandle = Core::Handle<RenderTargetBase>;
    }
}
