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

            void VOnLoaded() override;

			bool VSetData(std::string name, const void* data, size_t size) override;
			bool VSetInt(std::string name, int data) override;
			bool VSetFloat(std::string name, float data) override;
			bool VSetFloat2(std::string name, Math::Vector2 data) override;
			bool VSetFloat3(std::string name, Math::Vector3 data) override;
			bool VSetFloat4(std::string name, Math::Vector4 data) override;
			bool VSetMatrix3(std::string name, Math::Matrix3 data) override;
			bool VSetMatrix4(std::string name, Math::Matrix4 data) override;

			void Activate();

			void DeActivate();

		protected:
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

			void					UpdateAllBuffers();


			virtual void			VBind() = 0;
			virtual void			VUnbind() = 0;
			virtual bool			VInitShader() = 0;

			virtual bool			VSetShaderResourceView(std::string name, ID3D11ShaderResourceView* rv) = 0;
			virtual bool			VSetSamplerState(std::string name, ID3D11SamplerState* ss) = 0;
		};

	}
}