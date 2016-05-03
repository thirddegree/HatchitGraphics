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

#pragma once

#include <ht_platform.h>
#include <ht_texture.h>
#include <ht_singleton.h>
#include <ht_device.h>
#include <ht_material.h>
#include <ht_pipeline.h>
#include <ht_rootlayout.h>

namespace Hatchit
{
    namespace Graphics
    {
        class GPUResourceThread;

        class HT_API GPUResourcePool : public Core::Singleton<GPUResourcePool>
        {
        public:
            static bool             Initialize(IDevice* device);
            static void             DeInitialize();
            static bool             IsLocked();

            static void             RequestTexture(std::string file, void** data);
            static void             RequestMaterial(std::string file, void** data);
            static void             RequestRootLayout(std::string file, void** data);
            static void             RequestPipeline(std::string file, void** data);
            static void             RequestShader(std::string file, void** data);

            static void             RequestTextureAsync(TextureHandle _default, TextureHandle temporary, std::string file, void** data);
            static void             RequestMaterialAsync(MaterialHandle _default, MaterialHandle temporary, std::string file, void** data);
            static void             RequestRootLayoutAsync(RootLayoutHandle _default, RootLayoutHandle temporary, std::string file, void** data);
            static void             RequestPipelineAsync(PipelineHandle _default, PipelineHandle temporary, std::string file, void** data);
            static void             RequestShaderAsync(ShaderHandle _default, ShaderHandle temporary, std::string file, void** data);

            static void             CreateTexture(std::string file, void** data);
            static void             CreateMaterial(std::string file, void** data);
            static void             CreateRootLayout(std::string file, void** data);
            static void             CreatePipeline(std::string file, void** data);
            static void             CreateShader(std::string file, void** data);

        private:
            GPUResourceThread*  m_thread;
            IDevice*            m_device;
            
        };
    }
}
