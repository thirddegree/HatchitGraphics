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

            /* Initialize a render target from a JSON resource
            * \param resourceRenderTarget A handle to the JSON RenderTarget resource
            * \return True on success, false on failure
            */
            //virtual bool VInitFromResource(Resource::RenderTargetHandle resourceRenderTarget) = 0;

            ///Prepare the render target with a graphics language
            virtual bool VPrepare() = 0;

        protected:
            uint32_t m_width;
            uint32_t m_height;
        };

        using IRenderTargetHandle = Core::Handle<IRenderTarget>;
    }
}
