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

namespace Hatchit {

    namespace Graphics {

        class HT_API IRenderTarget
        {
        public:
			virtual ~IRenderTarget() { };

			///Override to bind the render target for reading with a graphics language
            virtual void VReadBind() = 0;
			///Override to bind the render target to be written to with a graphics language
            virtual void VWriteBind() = 0;
			///Override to free the render target from a graphics language
            virtual void VFree() = 0;
        };
    }
}
