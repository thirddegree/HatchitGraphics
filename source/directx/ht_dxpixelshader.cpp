/**
**    Hatchit Engine
**    Copyright(c) 2016 Third-Degree
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

#include <ht_dxpixelshader.h>

namespace Hatchit {

    namespace Graphics {

        DXPixelShader::DXPixelShader(ID3D11Device* device, ID3D11DeviceContext* context)
            : DXShader(device, context)
        {

        }

        DXPixelShader::~DXPixelShader()
        {

        }

		bool DXPixelShader::VSetShaderResourceView(std::string name, ID3D11ShaderResourceView* rv) { return true; }

		bool DXPixelShader::VSetSamplerState(std::string name, ID3D11SamplerState* ss) { return true; }

        bool DXPixelShader::VInitShader()
        {
            HRESULT hr = S_OK;

            BYTE* data = (BYTE*)m_data;

            DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
            // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
            // Setting this flag improves the shader debugging experience, but still allows
            // the shaders to be optimized and to run exactly the way they will run in
            // the release configuration of this program.
            dwShaderFlags |= D3DCOMPILE_DEBUG;

            // Disable optimizations to further improve shader debugging
            dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

            ID3DBlob* errorBlob = nullptr;
            hr = D3DCompile2(data, strlen((char*)data), nullptr, nullptr,
                nullptr, "main", "ps_5_0", NULL, NULL, NULL, NULL, NULL,
                &m_blob, &errorBlob);
            if (FAILED(hr))
            {
                if (errorBlob)
                {
                    OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
                    errorBlob->Release();
                }
                return false;
            }
            if (errorBlob) errorBlob->Release();

            // Create the vertex shader
            hr = m_device->CreatePixelShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader);
            if (FAILED(hr))
            {
                DirectX::ReleaseCOM(m_blob);
                return false;
            }

            return true;
        }

		void DXPixelShader::VBind()
		{
		
		}

		void DXPixelShader::VUnbind()
		{
		
		}

        ID3D11PixelShader* DXPixelShader::GetShader()
        {
            return m_shader;
        }
    }
}