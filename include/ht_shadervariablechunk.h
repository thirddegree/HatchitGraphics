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

/**
* \class ShaderVariableChunk
* \ingroup HatchitGraphics
*
* \brief An object used to store and manage shader variables
* this class will be used by mesh renderers, materials, and render passes
*/

#pragma once
#include <ht_shadervariable.h>
#include <ht_math.h>
#include <map>

namespace Hatchit {

    namespace Core
    {
        template<typename VarType>
        class Handle;
    }

    namespace Graphics {

        class HT_API ShaderVariableChunk
        {
        public:
            ShaderVariableChunk(std::map<std::string, Resource::ShaderVariable*> variables);
            ~ShaderVariableChunk();

            bool SetInt(std::string name, int data);
            bool SetFloat(std::string name, float data);
            bool SetFloat3(std::string name, Math::Vector3 data);
            bool SetFloat4(std::string name, Math::Vector4 data);
            bool SetMatrix4(std::string name, Math::Matrix4 data);

            bool BuildByteData();
            const BYTE* GetByteData();
            size_t GetSize();

        private:
            
            std::map<std::string, Resource::ShaderVariable*> m_variables;

            bool            m_dirty;
            BYTE*           m_byteData;
            std::size_t     m_byteDataSize;

        };
    }
}
