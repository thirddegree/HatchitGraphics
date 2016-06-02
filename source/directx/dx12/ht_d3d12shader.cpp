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

#include <ht_d3d12shader.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {

            D3D12Shader::D3D12Shader()
            {
                m_blob = nullptr;
            }

            D3D12Shader::~D3D12Shader()
            {
                ReleaseCOM(m_blob);
            }

            bool D3D12Shader::Initialize(Resource::ShaderHandle handle)
            {
                HRESULT hr = S_OK;

                hr = D3DCreateBlob(handle->GetBytecodeSize(), &m_blob);
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create shader blob.\n");
                    return false;
                }
                memcpy(m_blob->GetBufferPointer(), handle->GetBytecode(), handle->GetBytecodeSize());

                return true;
            }

            D3D12_SHADER_BYTECODE D3D12Shader::GetBytecode()
            {
                return CD3DX12_SHADER_BYTECODE(m_blob);
            }

        }
    }
}
