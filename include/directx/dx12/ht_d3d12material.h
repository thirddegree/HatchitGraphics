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
#include <ht_material.h>

namespace Hatchit {

    namespace Graphics {

        namespace DirectX 
        {
            class HT_API D3D12Material : public IMaterial
            {
            public:
                D3D12Material();

            private:

                // Inherited via IMaterial
                virtual void VOnLoaded() override;
                virtual bool VInitFromFile(Core::File * file) override;
                virtual bool VSetInt(std::string name, int data) override;
                virtual bool VSetFloat(std::string name, float data) override;
                virtual bool VSetFloat3(std::string name, Math::Vector3 data) override;
                virtual bool VSetFloat4(std::string name, Math::Vector4 data) override;
                virtual bool VSetMatrix4(std::string name, Math::Matrix4 data) override;
                virtual bool VBindTexture(std::string name, ITexture * texture) override;
                virtual bool VUnbindTexture(std::string name, ITexture * texture) override;
                virtual bool VPrepare() override;
                virtual bool VUpdate() override;
            };
        }
    }
}
