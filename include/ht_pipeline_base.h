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

#pragma once

#include <ht_platform.h>
#include <ht_string.h>      //std::string
#include <ht_math.h>        //Vectors and matricies
#include <ht_texture.h>     //TextureHandle
#include <ht_shadervariablechunk.h>

namespace Hatchit {

    namespace Core
    {
        template<typename VarType>
        class Handle;
    }

    namespace Resource
    {
        class Pipeline;
        class ShaderVariable;
    }

    namespace Graphics {

        class HT_API PipelineBase
        {
        public:
            virtual ~PipelineBase() {};

            virtual bool VSetShaderVariables(ShaderVariableChunk* variables) = 0;

            virtual bool VSetInt(size_t offset, int data) = 0;
            virtual bool VSetDouble(size_t offset, double data) = 0;
            virtual bool VSetFloat(size_t offset, float data) = 0;
            virtual bool VSetFloat2(size_t offset, Math::Vector2 data) = 0;
            virtual bool VSetFloat3(size_t offset, Math::Vector3 data) = 0;
            virtual bool VSetFloat4(size_t offset, Math::Vector4 data) = 0;
            virtual bool VSetMatrix4(size_t offset, Math::Matrix4 data) = 0;

            virtual bool VUpdate() = 0;
        protected:
            ShaderVariableChunk* m_shaderVariables;

            friend class Pipeline;
        };
    }
}