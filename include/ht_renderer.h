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

#pragma once

#include <ht_platform.h>
#include <ht_color.h>
#include <ht_types.h>

namespace Hatchit {

    namespace Graphics {

        enum class ClearArgs
        {
            Color,
            Depth,
            Stencil,
            ColorDepth,
            ColorStencil,
            ColorDepthStencil
        };

        enum RendererType
        {
            OPENGL,
            DIRECTX
        };

        struct RendererParams
        {
            RendererType renderer;
            void*        window;
        };


        class HT_API IRenderer
        {
        public:
            virtual ~IRenderer() { };

            virtual bool VInitialize(const RendererParams& params) = 0;
            virtual void VDeInitialize() = 0;
            virtual void VResizeBuffers(uint32_t width, uint32_t height) = 0;

            virtual void VSetClearColor(const Color& color) = 0;
            virtual void VClearBuffer(ClearArgs args) = 0;
            virtual void VPresent() = 0;
        };

    }
}
