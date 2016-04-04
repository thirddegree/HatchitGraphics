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
#include <ht_shader.h>
#include <unordered_map>

namespace Hatchit {
    
    namespace Graphics {

        namespace DX
        {

            class HT_API D3D12Shader : public IShader
            {
                typedef std::unordered_map<std::string, uint32_t> TextureTable;
                typedef std::unordered_map<std::string, uint32_t> SamplerTable;
            public:
                D3D12Shader();

                virtual ~D3D12Shader();

            private:
                uint32_t					m_constantBufferCount;
                ID3D12ShaderReflection* m_reflection;
                ID3DBlob*               m_blob;
                TextureTable            m_texTable;
                SamplerTable            m_samTable;
                uint32_t                m_cbCount;

                // Inherited via IShader
                //virtual bool VInitFromFile(Core::File * file) override;
          
                // Inherited via IShader
                //virtual void VOnLoaded() override;

                virtual bool VInitShader() = 0;

            };
        }
    }
}