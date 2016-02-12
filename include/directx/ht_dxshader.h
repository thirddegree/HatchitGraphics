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

#pragma once

#include <ht_platform.h>
#include <ht_directx.h>
#include <ht_dxtypes.h>
#include <ht_shader.h>
#include <unordered_map>

namespace Hatchit {

	namespace Graphics {

		class HT_API DXShader : public IShader
		{
			typedef std::unordered_map<std::string, ConstantBuffer*>	ConstantBufferTable;
			typedef std::unordered_map<std::string, uint32_t>			TextureTable;
			typedef std::unordered_map<std::string, uint32_t>			SampleTable;
		public:
			DXShader(ID3D11Device* device, ID3D11DeviceContext* context);

			virtual ~DXShader();

            virtual bool VInitShader() = 0;

            void VOnLoaded() override;

		private:
			uint32_t				m_constantBufferCount;
			ConstantBuffer*         m_constantBufferArray;
            SampleTable				m_sampleTable;
            TextureTable            m_textureTable;

            ID3D11Device*           m_device;
            ID3D11DeviceContext*    m_context;
            ID3DBlob*               m_blob;
            ID3D11ShaderReflection* m_reflection;
		};

	}
}