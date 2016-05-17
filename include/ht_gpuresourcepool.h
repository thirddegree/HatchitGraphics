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
*   \class GPUQueue
*   \ingroup HatchitGraphics
*
*   \brief Singelton class that manages loading of GPU resource objects.
*
*   The class implements the logic for loading GPU resources non-asynchronously
*   or asynchronously.
*/

#pragma once

#include <ht_platform.h>
#include <ht_texture.h>
#include <ht_singleton.h>
#include <ht_device.h>
#include <ht_material.h>
#include <ht_pipeline.h>
#include <ht_rootlayout.h>
#include <ht_rendertarget.h>
#include <ht_renderpass.h>
#include <ht_mesh.h>

namespace Hatchit
{
    namespace Graphics
    {
        class GPUResourceThread;
        class SwapChain;

        class HT_API GPUResourcePool : public Core::Singleton<GPUResourcePool>
        {
        public:
            static bool             Initialize(IDevice* device, SwapChain* swapchain);
            static void             DeInitialize();
            static bool             IsLocked();

            static void             RequestTexture(std::string file, void** data);
            static void             RequestMaterial(std::string file, void** data);
            static void             RequestRootLayout(std::string file, void** data);
            static void             RequestPipeline(std::string file, void** data);
            static void             RequestShader(std::string file, void** data);
            static void             RequestRenderPass(std::string file, void** data);
            static void             RequestRenderTarget(std::string file, void** data);
            static void             RequestMesh(std::string file, void** data);

            static void             RequestTextureAsync(std::string file, void** data);
            static void             RequestMaterialAsync(std::string file, void** data);
            static void             RequestRootLayoutAsync(std::string file, void** data);
            static void             RequestPipelineAsync(std::string file, void** data);
            static void             RequestShaderAsync(std::string file, void** data);
            static void             RequestRenderPassAsync(std::string file, void** data);
            static void             RequestRenderTargetAsync(std::string file, void** data);
            static void             RequestMeshAsync(std::string file, void** data);

            static void             CreateTexture(std::string file, void** data);
            static void             CreateMaterial(std::string file, void** data);
            static void             CreateRootLayout(std::string file, void** data);
            static void             CreatePipeline(std::string file, void** data);
            static void             CreateShader(std::string file, void** data);
            static void             CreateRenderPass(std::string file, void** data);
            static void             CreateRenderTarget(std::string file, void** data);
            static void             CreateMesh(std::string file, void** data);

        private:
            GPUResourceThread*  m_thread;
            IDevice*            m_device;
            
        };
    }
}
