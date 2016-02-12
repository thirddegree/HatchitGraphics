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
			typedef std::unordered_map<std::string, ConstantBuffer*>		ConstantBufferTable;
			typedef std::unordered_map<std::string, uint32_t>				TextureTable;
			typedef std::unordered_map<std::string, uint32_t>				SampleTable;
			typedef std::unordered_map<std::string, ConstantBufferVariable> ConstantBufferVariableTable;
		public:
			DXShader(ID3D11Device* device, ID3D11DeviceContext* context);

			virtual ~DXShader();

            virtual bool VInitShader() = 0;

            void VOnLoaded() override;

			bool VSetData(std::string name, const void* data, size_t size) override;
			bool VSetInt(std::string name, int data) override;
			bool VSetFloat(std::string name, float data) override;
			bool VSetFloat2(std::string name, const float data[2]) override;
			bool VSetFloat2(std::string name, float x, float y) override;
			bool VSetFloat3(std::string name, const float data[3]) override;
			bool VSetFloat3(std::string name, float x, float y, float z) override;
			bool VSetFloat4(std::string name, const float data[4]) override;
			bool VSetFloat4(std::string name, float x, float y, float z, float w) override;
			bool VSetMatrix4x4(std::string name, const float data[16]) override;

		private:
			uint32_t					m_constantBufferCount;
			ConstantBuffer*				m_constantBufferArray;
			ConstantBufferVariableTable m_constantBufferVarTable;
            SampleTable					m_sampleTable;
            TextureTable				m_textureTable;

            ID3D11Device*           m_device;
            ID3D11DeviceContext*    m_context;
            ID3DBlob*               m_blob;
            ID3D11ShaderReflection* m_reflection;



		protected:
			ConstantBufferVariable* FindVariable(std::string name, size_t size);
			ConstantBuffer*			FindBuffer(std::string name);
			uint32_t				FindTextureBindIndex(std::string name);
			uint32_t				FindSampleBindIndex(std::string name);
		};

	}
}