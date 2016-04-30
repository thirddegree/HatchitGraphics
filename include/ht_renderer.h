/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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
* \defgroup HatchitGraphics
*/

/**
* \class IRenderer
* \ingroup HatchitGraphics
*
* \brief An interface to a renderer that will need to be implemented with a graphics language
*
* A manager class that handles rendering objects into render passes and is
* responsible for throwing the final frame onto the screen by utilizing a
* graphics language
*/

#pragma once

#include <ht_platform.h>
#include <ht_color.h>
#include <ht_types.h>
#include <ht_string.h>
#include <ht_renderpass.h>
#include <ht_camera.h>
#include <ht_device.h>

namespace Hatchit {

    namespace Graphics {

        class SwapChain;

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
            UNKNOWN,
            OPENGL,
            DIRECTX11,
            DIRECTX12,
            VULKAN
        };

        struct RendererParams
        {
            RendererType    renderer;
            bool            validate;
            void*           window;
            uint32_t        viewportWidth;
            uint32_t        viewportHeight;
            void*           display;
            Color           clearColor;
            std::string     applicationName;
        };

        class HT_API Renderer
        {
        public:
            Renderer();

            ~Renderer();
            
            /** Initialize the renderer
            * \param params The paramaters to intialize this renderer with
            */
            bool Initialize(const RendererParams& params);

            /** Resizes the the screen
            * \param width The new width of the screen
            * \param height The new height of the screen
            */
            void ResizeBuffers(uint32_t width, uint32_t height);

            ///Render all render passes
            void Render();

            ///Present a frame to the screen via a backbuffer
            void Present();

            void RegisterRenderPass(RenderPassBaseHandle pass);

            void RegisterCamera(Camera camera);

            static IDevice* const GetDevice();

            static RendererType GetType();

        protected:
            static IDevice*     _Device;
            static RendererType _Type;

            //A collection of renderpass layers. Each layer may contain multiple render passes.
            std::vector<std::vector<RenderPassBaseHandle>> m_renderPassLayers = std::vector<std::vector<RenderPassBaseHandle>>(64);
            //A collection of cameras sorted by renderpass layer. Repopulated each frame.
            std::vector<std::vector<Graphics::Camera>> m_renderPassCameras = std::vector<std::vector<Graphics::Camera>>(64);
            
            SwapChain* m_swapChain;

            TextureHandle test;
        };

    }
}
