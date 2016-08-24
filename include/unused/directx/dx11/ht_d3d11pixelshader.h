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
#include <ht_d3d11shader.h>

namespace Hatchit {

	namespace Graphics {

        namespace DirectX {

            class HT_API D3D11PixelShader : public D3D11Shader
            {
            public:
                D3D11PixelShader(ID3D11Device* device, ID3D11DeviceContext* context);

                ~D3D11PixelShader();


                ID3D11PixelShader* GetShader();

                bool VSetShaderResourceView(std::string name, ID3D11ShaderResourceView* rv) override;

                bool VSetSamplerState(std::string name, ID3D11SamplerState* ss)             override;

            private:
                ID3D11PixelShader* m_shader;

            protected:
                void VBind()        override;
                void VUnbind()      override;
                bool VInitShader()  override;

            };
        }
	}
}
