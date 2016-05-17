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
#include <ht_directx.h>
#include <ht_material_base.h>
#include <ht_material_resource.h>

namespace Hatchit {

    namespace Core
    {
        template<typename VarType>
        class Handle;
    }

    namespace Resource
    {
        class Material;
    }

    namespace Graphics {

        class Material;
        namespace DX
        {
            class D3D12Device;

            class HT_API D3D12Material : public MaterialBase
            {
            public:
                D3D12Material();

                ~D3D12Material();

                bool VBindTexture(std::string name, TextureHandle texture)      override;
                bool VUnbindTexture(std::string name, TextureHandle texture)    override;
                bool VUpdate()                                                  override;
                const PipelineHandle VGetPipeline() const                       override;

            private:
                bool Initialize(Resource::MaterialHandle handle, D3D12Device* device);

                friend class D3D12GPUResourceThread;
            };
        }
    }
}
