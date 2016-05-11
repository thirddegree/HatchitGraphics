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
#include <ht_shadervariable.h>

namespace Hatchit
{
    namespace Graphics
    {
        /* Constructs a Shader Variable Chunk
        */
        ShaderVariableChunk::ShaderVariableChunk(std::vector<Resource::ShaderVariable*> variables)
        {
            //count byte size of data passed in
            m_byteDataSize = 0;
            for (size_t i = 0; i < variables.size(); i++)
            {
                //increment size counter
                Resource::ShaderVariable::Type type = variables[i]->GetType();
                m_byteDataSize += Resource::ShaderVariable::SizeFromType(type);
            }
            //now that we've counted the bytes, allocate enough space
            m_byteData = new BYTE[m_byteDataSize];


            //restart, this time adding the data to the array
            m_byteDataSize = 0;
            for (size_t i = 0; i < variables.size(); i++)
            {
                Resource::ShaderVariable::Type type = variables[i]->GetType();
                size_t offset = Resource::ShaderVariable::SizeFromType(type);
                //add the data again for real this time
                memcpy(m_byteData + m_byteDataSize, variables[i]->GetData(), offset);
                //increment size counter
                m_byteDataSize += offset;
            }
        };

        ShaderVariableChunk::~ShaderVariableChunk()
        {
            delete[] m_byteData;
        };


        bool ShaderVariableChunk::SetInt(size_t offset, uint32_t data)
        {
            BYTE* location = m_byteData + offset;
            assert(location + sizeof(uint32_t) <= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(uint32_t));
            return true;
        }

        bool ShaderVariableChunk::SetFloat(size_t offset, float data)
        {
            BYTE* location = m_byteData + offset;
            assert(location + sizeof(float) <= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float));
            return true;
        }

        bool ShaderVariableChunk::SetFloat3(size_t offset, Math::Vector3 data)
        {
            BYTE* location = m_byteData + offset;
            assert(location + sizeof(float) * 3 <= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float) * 3);
            return true;
        }

        bool ShaderVariableChunk::SetFloat4(size_t offset, Math::Vector4 data)
        {
            BYTE* location = m_byteData + offset;
            assert(location + sizeof(float) * 4 <= m_byteData + m_byteDataSize);
            memcpy(location, &data, sizeof(float) * 4);
            return true;
        }

        bool ShaderVariableChunk::SetMatrix4(size_t offset, Math::Matrix4 data)
        {
            BYTE* location = m_byteData + offset;
            assert(location + sizeof(float) * 16 <= m_byteData + m_byteDataSize);
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
