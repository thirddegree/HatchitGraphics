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

#include <ht_d3d12material.h>

namespace Hatchit {
    
    namespace Graphics {

        namespace DirectX
        {
            D3D12Material::D3D12Material()
            {

            }

            void D3D12Material::VOnLoaded()
            {
            }

            bool D3D12Material::VInitFromFile(Core::File * file)
            {
                return false;
            }

            bool D3D12Material::VSetInt(std::string name, int data)
            {
                return false;
            }

            bool D3D12Material::VSetFloat(std::string name, float data)
            {
                return false;
            }

            bool D3D12Material::VSetFloat3(std::string name, Math::Vector3 data)
            {
                return false;
            }

            bool D3D12Material::VSetFloat4(std::string name, Math::Vector4 data)
            {
                return false;
            }

            bool D3D12Material::VSetMatrix4(std::string name, Math::Matrix4 data)
            {
                return false;
            }

            bool D3D12Material::VBindTexture(std::string name, ITexture * texture)
            {
                return false;
            }

            bool D3D12Material::VUnbindTexture(std::string name, ITexture * texture)
            {
                return false;
            }

            bool D3D12Material::VPrepare()
            {
                return false;
            }

            bool D3D12Material::VUpdate()
            {
                return false;
            }

        }
    }
}