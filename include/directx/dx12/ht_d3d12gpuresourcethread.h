
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
#include <ht_texture_resource.h>
#include <ht_material_resource.h>
#include <ht_rootlayout_resource.h>
#include <ht_gpuresourcethread.h>
#include <ht_gpuresourcerequest.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class D3D12Device;

            class HT_API D3D12GPUResourceThread : public GPUResourceThread
            {
            public:
                D3D12GPUResourceThread(D3D12Device* device);

                ~D3D12GPUResourceThread();

                void VStart()   override;

                void VCreateTexture(std::string file, void** data)      override;
                void VCreateMaterial(std::string file, void** data)     override;
                void VCreateRootLayout(std::string file, void** data)   override;
                void VCreatePipeline(std::string file, void** data)     override;
                void VCreateShader(std::string file, void** data)       override;

            private:
                D3D12Device*            m_device;

                void ProcessTextureRequest(TextureRequest* request);
                void ProcessMaterialRequest(MaterialRequest* request);
                void ProcessRootLayoutRequest(RootLayoutRequest* request);
                void ProcessPipelineRequest(PipelineRequest* request);
                void ProcessShaderRequest(ShaderRequest* request);

                void CreateTextureBase(Resource::TextureHandle handle, void** base);
                void CreateMaterialBase(Resource::MaterialHandle handle, void** base);
                void CreateRootLayoutBase(Resource::RootLayoutHandle handle, void** base);
                void CreatePipelineBase(Resource::PipelineHandle handle, void** base);
                void CreateShaderBase(Resource::ShaderHandle handle, void** base);

                void thread_main();
            };
        }
    }
}