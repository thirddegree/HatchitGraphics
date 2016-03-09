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

#include <ht_d3d11vertexshader.h>
#include <ht_debug.h>

namespace Hatchit {

	namespace Graphics {

        namespace DirectX {


            D3D11VertexShader::D3D11VertexShader(ID3D11Device* device, ID3D11DeviceContext* context)
                : D3D11Shader(device, context)
            {
                m_shader = nullptr;
                m_inputLayout = nullptr;
            }

            D3D11VertexShader::~D3D11VertexShader()
            {
                DirectX::ReleaseCOM(m_shader);
                DirectX::ReleaseCOM(m_inputLayout);
            }

            bool D3D11VertexShader::VInitShader()
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
                hr = D3DCompile2(data, strlen((char*)data), nullptr, nullptr, nullptr, "main", "vs_5_0", dwShaderFlags, 0, 0, 0, 0, &m_blob, &errorBlob);
                if (FAILED(hr))
                {
                    if (errorBlob)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("DXVertexShader::VInitShader() Error: %s\n", errorBlob->GetBufferPointer());
                        DirectX::ReleaseCOM(errorBlob);
#endif
                    }
                    return false;
                }
                if (errorBlob)
                    DirectX::ReleaseCOM(errorBlob);

                //Create the vertex shader
                hr = m_device->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader);
                if (FAILED(hr))
                {
                    DirectX::ReleaseCOM(m_blob);
                    return false;
                }

                //Reflect shader info
                ID3D11ShaderReflection* reflection;
                D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), __uuidof(ID3D11ShaderReflection), reinterpret_cast<void**>(&reflection));

                //Get shader info
                D3D11_SHADER_DESC shaderDesc;
                reflection->GetDesc(&shaderDesc);

                //Read input layout description from shader info
                std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
                for (uint32_t i = 0; i < shaderDesc.InputParameters; i++)
                {
                    D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
                    reflection->GetInputParameterDesc(i, &paramDesc);

                    //Fill out element desc
                    D3D11_INPUT_ELEMENT_DESC elementDesc;
                    elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    elementDesc.InputSlot = 0;
                    elementDesc.SemanticName = paramDesc.SemanticName;
                    elementDesc.SemanticIndex = paramDesc.SemanticIndex;

                    elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
                    elementDesc.InstanceDataStepRate = 0;

                    // Determine DXGI format
                    if (paramDesc.Mask == 1)
                    {
                        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
                    }
                    else if (paramDesc.Mask <= 3)
                    {
                        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                    }
                    else if (paramDesc.Mask <= 7)
                    {
                        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                    }
                    else if (paramDesc.Mask <= 15)
                    {
                        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                    }

                    // Save element desc
                    inputLayoutDesc.push_back(elementDesc);
                }

                //Try to create input layout
                hr = m_device->CreateInputLayout(&inputLayoutDesc[0],
                    static_cast<uint32_t>(inputLayoutDesc.size()), m_blob->GetBufferPointer(), m_blob->GetBufferSize(), &m_inputLayout);
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("DXVertexShader::VInitShader() Error: Failed to create input layout.\n");
#endif
                    false;
                }

                return true;
            }

            void D3D11VertexShader::VBind()
            {
                m_context->IASetInputLayout(m_inputLayout);
                m_context->VSSetShader(m_shader, 0, 0);

                for (uint32_t i = 0; i < m_constantBufferCount; i++)
                    m_context->VSSetConstantBuffers(m_constantBufferArray[i].bindIndex, 1, &m_constantBufferArray[i].buffer);
            }

            void D3D11VertexShader::VUnbind()
            {
                m_context->VSSetSamplers(0, 0, 0);
            }

            bool D3D11VertexShader::VSetShaderResourceView(std::string name, ID3D11ShaderResourceView* rv)
            {
                //Find variable
                int32_t index = FindTextureBindIndex(name);
                if (index == -1)
                    return false;

                //set resource view
                m_context->VSSetShaderResources(index, 1, &rv);

                return true;
            }

            bool D3D11VertexShader::VSetSamplerState(std::string name, ID3D11SamplerState* ss)
            {
                //Find variable
                int32_t index = FindSampleBindIndex(name);
                if (index == -1)
                    return false;

                //set sampler
                m_context->VSSetSamplers(index, 1, &ss);

                return true;
            }

            ID3D11VertexShader* D3D11VertexShader::GetShader()
            {
                return m_shader;
            }

            ID3D11InputLayout* D3D11VertexShader::GetLayout()
            {
                return m_inputLayout;
            }
        }

	}
}