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

                bool VSetInt(std::string name, int data)                        override;
                bool VSetFloat(std::string name, float data)                    override;
                bool VSetFloat3(std::string name, Math::Vector3 data)           override;
                bool VSetFloat4(std::string name, Math::Vector4 data)           override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data)          override;
                bool VBindTexture(std::string name, TextureHandle texture)      override;
                bool VUnbindTexture(std::string name, TextureHandle texture)    override;
                bool VUpdate()                                                  override;

            private:
                bool Initialize(Resource::MaterialHandle handle, D3D12Device* device);

                friend class D3D12GPUResourceThread;
            };
        }
    }
}
