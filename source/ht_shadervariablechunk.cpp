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
            //count byte size of data passed in
            m_byteDataSize = 0;
            std::map<std::string, Resource::ShaderVariable*>::iterator itr;
            for (itr = variables.begin(); itr != variables.end(); itr++)
            {
                //increment size counter
                Resource::ShaderVariable::Type type = itr->second->GetType();
                m_byteDataSize += Resource::ShaderVariable::SizeFromType(type);
            }
            //now that we've counted the bytes, allocate enough space
            m_byteData = new BYTE[m_byteDataSize];


            //restart, this time adding the data to the array
            m_byteDataSize = 0;
            for (itr = variables.begin(); itr != variables.end(); itr++)
            {
                Resource::ShaderVariable::Type type = itr->second->GetType();
                size_t offset = Resource::ShaderVariable::SizeFromType(type);
                //new pointer to that location in the byte data
                m_variables.emplace(itr->first, m_byteData + m_byteDataSize);
                //add the data again for real this time
                memcpy(m_byteData + m_byteDataSize, itr->second->GetData(), offset);
                //increment size counter
                m_byteDataSize += offset;
            }
        };

        ShaderVariableChunk::~ShaderVariableChunk()
        {
            delete[] m_byteData;
        };


        bool ShaderVariableChunk::SetInt(std::string name, uint32_t data)
        {
            BYTE* location = m_variables.at(name);
            assert(location + sizeof(uint32_t) >= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(uint32_t));
            return true;
        }

        bool ShaderVariableChunk::SetFloat(std::string name, float data)
        {
            BYTE* location = m_variables.at(name);
            assert(location + sizeof(float) >= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float));
            return true;
        }

        bool ShaderVariableChunk::SetFloat3(std::string name, Math::Vector3 data)
        {
            BYTE* location = m_variables.at(name);
            assert(location + sizeof(float) * 3 >= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float) * 3);
            return true;
        }

        bool ShaderVariableChunk::SetFloat4(std::string name, Math::Vector4 data)
        {
            BYTE* location = m_variables.at(name);
            assert(location + sizeof(float) * 4 >= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float) * 4);
            return true;
        }

        bool ShaderVariableChunk::SetMatrix4(std::string name, Math::Matrix4 data)
        {
            BYTE* location = m_variables.at(name);
            assert(location + sizeof(float) * 16 >= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float) * 16);
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