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

#include <ht_dxshader.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        DXShader::DXShader(ID3D11Device* device, ID3D11DeviceContext* context)
        {
            m_device = device;
            m_context = context;
            m_reflection = nullptr;
            m_blob = nullptr;
        }

        DXShader::~DXShader()
        {
            /*Release all constant buffers and delete local buffer data*/
            for (uint32_t i = 0; i < m_constantBufferCount; i++)
            {
                m_constantBufferArray[i].buffer->Release();
                delete[] m_constantBufferArray[i].data;
            }
            delete[] m_constantBufferArray;

            m_constantBufferCount = 0;
        }

        void DXShader::VOnLoaded()
        {
            //Initialize shader with data
            if (!this->VInitShader())
            {
#ifdef _DEBUG
                Core::DebugPrintF("DXShader::VOnLoaded, Failed to initialize shader.\n");
#endif
                return;
            }

            if (!m_reflection)
            {
                //Reflect shader info
                D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(),
                    __uuidof(ID3D11ShaderReflection), reinterpret_cast<void**>(&m_reflection));
            }

            //Get description of the shader
            D3D11_SHADER_DESC desc;
            m_reflection->GetDesc(&desc);

            //Create array of constant buffers
            m_constantBufferCount = desc.ConstantBuffers;
            m_constantBufferArray = new ConstantBuffer[m_constantBufferCount];

            //Handle bound resources (shaders of samples)
            uint32_t count = desc.BoundResources;
            for (uint32_t i = 0; i < count; i++)
            {
                //Get the resource description
                D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
                m_reflection->GetResourceBindingDesc(i, &resourceDesc);

                //Check what type
                switch (resourceDesc.Type)
                {
                case D3D_SIT_TEXTURE:
                    m_textureTable.insert(std::pair<std::string, uint32_t>(resourceDesc.Name, resourceDesc.BindCount));
                    break;
                    
                case D3D_SIT_SAMPLER:
                    m_sampleTable.insert(std::pair<std::string, uint32_t>(resourceDesc.Name, resourceDesc.BindCount));
                    break;

                case D3D_SIT_STRUCTURED:
                    m_textureTable.insert(std::pair<std::string, uint32_t>(resourceDesc.Name, resourceDesc.BindCount));
                    break;
                }
            }

            //Loop over all constant buffers
            for (uint32_t i = 0; i < m_constantBufferCount; i++)
            {
                //Get the buffer
                ID3D11ShaderReflectionConstantBuffer* reflectBuffer = m_reflection->GetConstantBufferByIndex(i);

                //Get the description of buffer
                D3D11_SHADER_BUFFER_DESC bufferDesc;
                reflectBuffer->GetDesc(&bufferDesc);

                //Get description of resource 
                D3D11_SHADER_INPUT_BIND_DESC bindDesc;
                m_reflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);
                

                //set up buffer and putits pointer into table
                m_constantBufferArray[i].bindIndex = bindDesc.BindPoint;
            
            }
        }
    }
}