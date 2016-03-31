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

#include <ht_d3d12shader.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {

            D3D12Shader::D3D12Shader()
            {
                m_reflection = nullptr;
                m_blob = nullptr;
                m_cbCount = 0;
            }

            D3D12Shader::~D3D12Shader()
            {

            }

            //bool D3D12Shader::VInitFromFile(Core::File * file)
            //{
            //    //size_t size = file->SizeBytes();
            //    //m_data = new BYTE[size];
            //    //file->Read(static_cast<BYTE*>(m_data), size);

            //    return false;
            //}

//            void D3D12Shader::VOnLoaded()
//            {
//                //Initialize shader with data
//                if (!this->VInitShader())
//                {
//#ifdef _DEBUG
//                    Core::DebugPrintF("D3D12Shader::VOnLoaded, Failed to initialize shader.\n");
//#endif
//                    return;
//                }
//
//                if (!m_reflection)
//                {
//                    //Reflect shader info
//                    D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(),
//                        __uuidof(ID3D12ShaderReflection), reinterpret_cast<void**>(&m_reflection));
//                }
//
//                //Get description of shader
//                D3D12_SHADER_DESC desc;
//                m_reflection->GetDesc(&desc);
//
//                //Create array of constant buffers
//                //
//                //
//
//                //Handle bound resources
//                uint32_t count = desc.BoundResources;
//                for (uint32_t i = 0; i < count; i++)
//                {
//                    //Get the resource description
//                    D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
//                    m_reflection->GetResourceBindingDesc(i, &resourceDesc);
//
//                    //Check type
//                    switch (resourceDesc.Type)
//                    {
//                    case D3D_SIT_TEXTURE:
//                    case D3D_SIT_STRUCTURED:
//                        m_texTable.insert(std::make_pair(resourceDesc.Name, resourceDesc.BindCount));
//                        break;
//                    case D3D_SIT_SAMPLER:
//                        m_samTable.insert(std::make_pair(resourceDesc.Name, resourceDesc.BindCount));
//                        break;
//                    }
//                }
//
//                //Loop over all constant buffers
//                for (uint32_t i = 0; i < m_cbCount; i++)
//                {
//
//                }
//            }

        }
    }
}