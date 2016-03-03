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
* \class ISwapchain
* \ingroup HatchitGraphics
*
* \brief An interface to a swapchain system that can be extended an implemented in a graphics language
*
* This class will be extended by another class that will implement a swapchain.
* A swapchain is the chain of swapbuffers used for drawing to. Each buffer will need 
* a framebuffer that actually gets written to.
*/

#pragma once

#include <ht_platform.h>
#include <ht_rendertarget.h>
#include <ht_pipeline.h>
#include <ht_renderer.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API ISwapchain
        {
        public:
            virtual ~ISwapchain() {}

            void SetIncomingRenderTarget(IRenderTarget* inputTexture);

            uint32_t GetWidth();
            uint32_t GetHeight();

        protected:
            //Input
            IRenderTarget* m_inputTexture;

            //For rendering
            IPipeline* m_pipeline;
            uint32_t m_currentBuffer;
            uint32_t m_width, m_height;
        };
    }
}
