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

			//clear tables
			m_constantBufferVarTable.clear();
			m_textureTable.clear();
			m_sampleTable.clear();
        }

        bool DXShader::VInitFromFile(Core::File* file)
        {
            size_t size = file->SizeBytes();
            m_data = new BYTE[size];
            file->Read(static_cast<BYTE*>(m_data), size);

            return true;
        }

		ConstantBufferVariable* DXShader::FindVariable(std::string name, size_t size)
		{
			ConstantBufferVariableTable::iterator it = m_constantBufferVarTable.find(name);
			if (it == m_constantBufferVarTable.end())
				return nullptr;

			ConstantBufferVariable* variable = &it->second;
			if (variable->size != size)
				return nullptr;

			return variable;
		}

		ConstantBuffer* DXShader::FindBuffer(std::string name)
		{
			for (uint32_t i = 0; i < m_constantBufferCount; i++)
			{
				ConstantBuffer* buffer = &m_constantBufferArray[i];
				if (buffer->id == name)
					return buffer;
			}

			return nullptr;
		}

		int32_t DXShader::FindTextureBindIndex(std::string name)
		{
			TextureTable::iterator it = m_textureTable.find(name);
			if (it == m_textureTable.end())
				return -1;

			return it->second;
		}

		int32_t DXShader::FindSampleBindIndex(std::string name)
		{
			SampleTable::iterator it = m_sampleTable.find(name);
			if (it == m_sampleTable.end())
				return -1;

			return it->second;
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
				
				//Create this constant buffer
				D3D11_BUFFER_DESC _newDesc;
				_newDesc.Usage = D3D11_USAGE_DEFAULT;
				_newDesc.ByteWidth = bufferDesc.Size;
				_newDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				_newDesc.CPUAccessFlags = 0;
				_newDesc.MiscFlags = 0;
				_newDesc.StructureByteStride = 0;
				
				m_device->CreateBuffer(&_newDesc, 0, &m_constantBufferArray[i].buffer);

				//Setup the data buffer for constant buffer
				m_constantBufferArray[i].data = new BYTE[bufferDesc.Size];
				ZeroMemory(&m_constantBufferArray[i].data, bufferDesc.Size);
				m_constantBufferArray[i].id = bufferDesc.Name;

				//Iterate over all variables in this buffer
				for (uint32_t i = 0; i < bufferDesc.Variables; i++)
				{
					//Get variable
					ID3D11ShaderReflectionVariable* var = reflectBuffer->GetVariableByIndex(i);

					//Get variable description
					D3D11_SHADER_VARIABLE_DESC varDesc;
					var->GetDesc(&varDesc);

					//Create the variable struct
					ConstantBufferVariable variable;
					variable.constantBufferIndex = i;
					variable.byteOffset = varDesc.StartOffset;
					variable.size = varDesc.Size;

					//Get the string representation
					std::string name = varDesc.Name;

					//add variable to table
					m_constantBufferVarTable.insert(std::pair<std::string, ConstantBufferVariable>(name, variable));
				}
            }

        }

		void DXShader::Activate()
		{
			UpdateAllBuffers();

			VBind();
		}

		void DXShader::DeActivate()
		{
			VUnbind();
		}

		void DXShader::UpdateAllBuffers()
		{
			for (uint32_t i = 0; i < m_constantBufferCount; i++)
			{
				ConstantBuffer* cb = &m_constantBufferArray[i];

				m_context->UpdateSubresource(cb->buffer, 0, 0, cb->data, 0, 0);
			}
		}

		bool DXShader::VSetData(std::string name, const void* data, size_t size)
		{
			//grab variable by name
			ConstantBufferVariable* variable = FindVariable(name, size);
			if (variable == nullptr)
				return false;

			//copy data into the buffer
			memcpy(m_constantBufferArray[variable->constantBufferIndex].data + variable->byteOffset,
				data, size);

			return true;
		}

		bool DXShader::VSetInt(std::string name, int data)
		{
			return VSetData(name, static_cast<void*>(&data), sizeof(int));
		}

		bool DXShader::VSetFloat(std::string name, float data)
		{
			return VSetData(name, static_cast<void*>(&data), sizeof(float));
		}

		bool DXShader::VSetFloat2(std::string name, Math::Vector2 data)
		{
			return VSetData(name, static_cast<void*>(data.getAsArray()), sizeof(float) * 2);
		}

		bool DXShader::VSetFloat3(std::string name, Math::Vector3 data)
		{
			return VSetData(name, static_cast<void*>(data.getAsArray()), sizeof(float) * 3);
		}

		bool DXShader::VSetFloat4(std::string name, Math::Vector4 data)
		{
			return VSetData(name, static_cast<void*>(data.getAsArray()), sizeof(float) * 4);
		}

		bool DXShader::VSetMatrix3(std::string name, Math::Matrix3 data)
		{
			return VSetData(name, static_cast<void*>(data.getAsArray()), sizeof(float) * 12);
		}

		bool DXShader::VSetMatrix4(std::string name, Math::Matrix4 data)
		{
			return VSetData(name, static_cast<const void*>(data.getAsArray()), sizeof(float) * 16);
		}
    }
}