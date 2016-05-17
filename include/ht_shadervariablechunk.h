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
#include <unordered_map>

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
            ShaderVariableChunk(std::vector<Resource::ShaderVariable*> variables);
            ~ShaderVariableChunk();

            bool SetInt(size_t offset, uint32_t data);
            bool SetDouble(size_t offset, double data);
            bool SetFloat(size_t offset, float data);
            bool SetFloat2(size_t offset, Math::Vector2 data);
            bool SetFloat3(size_t offset, Math::Vector3 data);
            bool SetFloat4(size_t offset, Math::Vector4 data);
            bool SetMatrix4(size_t offset, Math::Matrix4 data);

            const BYTE* GetByteData();
            size_t GetSize();

        private:
            BYTE*           m_byteData;
            std::size_t     m_byteDataSize;

        };
    }
}
