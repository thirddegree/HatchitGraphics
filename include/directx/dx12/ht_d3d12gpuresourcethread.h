
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

            private:
                D3D12Device*            m_device;

                void VCreateTextureBase(Resource::TextureHandle handle, void** base)            override;
                void VCreateMaterialBase(Resource::MaterialHandle handle, void** base)          override;
                void VCreateRootLayoutBase(Resource::RootLayoutHandle handle, void** base)      override;
                void VCreatePipelineBase(Resource::PipelineHandle handle, void** base)          override;
                void VCreateShaderBase(Resource::ShaderHandle handle, void** base)              override;
                void VCreateRenderPassBase(Resource::RenderPassHandle handle, void** base)      override;
                void VCreateRenderTargetBase(Resource::RenderTargetHandle handle, void** base)  override;
                void VCreateMeshBase(Resource::ModelHandle handle, void** base)                 override;

                void thread_main();
            };
        }
    }
}