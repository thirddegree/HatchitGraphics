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

#include <ht_platform.h>
#include <ht_renderer.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API GLRenderer : public IRenderer
        {
        public:
            GLRenderer();

            ~GLRenderer();

            bool VInitialize(const RendererParams& params)                      override;

            void VDeInitialize()                                                override;

            void VResizeBuffers(uint32_t width, uint32_t height)                override;

            void VSetClearColor(const Color& color)                             override;

            void VClearBuffer(ClearArgs args)                                   override;

            void VPresent()                                                     override;

        private:
        };

    }

}
