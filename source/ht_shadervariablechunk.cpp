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

#include <ht_shadervariablechunk.h>
#include <ht_shadervariable.h>>

namespace Hatchit
{
    namespace Graphics
    {
        /* Constructs a Shader Variable Chunk
        */
        ShaderVariableChunk::ShaderVariableChunk(std::map<std::string, Resource::ShaderVariable*> variables)
        {
            m_variables = variables;


            //also need to allocate byte array based on size here
            m_byteDataSize = 0;

            std::map<std::string, Resource::ShaderVariable*>::iterator itr;
            for (itr = m_variables.begin(); itr != m_variables.end(); itr++)
            {
                Resource::ShaderVariable::Type type = itr->second->GetType();
                //size of the object being added
                m_byteDataSize += Resource::ShaderVariable::SizeFromType(type);
            }


            m_byteData = new BYTE[m_byteDataSize];


            m_dirty = true;
        };

        ShaderVariableChunk::~ShaderVariableChunk()
        {
            std::map<std::string, Resource::ShaderVariable*>::iterator itr;
            for (itr = m_variables.begin(); itr != m_variables.end(); itr++)
            {
                delete itr->second;
            }
            delete[] m_byteData;
        };


        bool ShaderVariableChunk::SetInt(std::string name, int data)
        {
            static_cast<Resource::IntVariable*>(m_variables[name])->SetData(data);
            m_dirty = true;
            return true;
        }

        bool ShaderVariableChunk::SetFloat(std::string name, float data)
        {
            static_cast<Resource::FloatVariable*>(m_variables[name])->SetData(data);
            m_dirty = true;
            return true;
        }

        bool ShaderVariableChunk::SetFloat3(std::string name, Math::Vector3 data)
        {
            static_cast<Resource::Float3Variable*>(m_variables[name])->SetData(data);
            m_dirty = true;
            return true;
        }

        bool ShaderVariableChunk::SetFloat4(std::string name, Math::Vector4 data)
        {
            static_cast<Resource::Float4Variable*>(m_variables[name])->SetData(data);
            m_dirty = true;
            return true;
        }

        bool ShaderVariableChunk::SetMatrix4(std::string name, Math::Matrix4 data)
        {
            static_cast<Resource::Matrix4Variable*>(m_variables[name])->SetData(data);
            m_dirty = true;
            return true;
        }

        //rebuilds the shadervariable chunk if it's changed
        bool ShaderVariableChunk::BuildByteData()
        {
            if (!m_dirty) return true;
            //actually construct the byte data

            //number of bytes into the data we have filled so far
            size_t offset = 0;

            std::map<std::string, Resource::ShaderVariable*>::iterator itr;
            for (itr = m_variables.begin(); itr != m_variables.end(); itr++)
            {
                Resource::ShaderVariable::Type type = itr->second->GetType();
                //size of the object being added
                size_t size = Resource::ShaderVariable::SizeFromType(type);
                //copy (size) bytes of data from shadervariable to our byte blob
                assert(offset + size <= m_byteDataSize);
                memcpy(m_byteData + offset, itr->second->GetData(), size);
                offset += size;
            }


            //done filling data
            m_dirty = false;
            return true;
        }

        const BYTE * ShaderVariableChunk::GetByteData()
        {
            return m_byteData;
        }

        size_t ShaderVariableChunk::GetSize()
        {
            return m_byteDataSize;
        }
    }
}