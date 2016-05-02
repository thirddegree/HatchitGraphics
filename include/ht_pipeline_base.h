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

            virtual bool VAddShaderVariables(std::map<std::string, Hatchit::Resource::ShaderVariable*> shaderVariables) = 0;

            virtual bool VSetInt(std::string name, int data) = 0;
            virtual bool VSetDouble(std::string name, double data) = 0;
            virtual bool VSetFloat(std::string name, float data) = 0;
            virtual bool VSetFloat2(std::string name, Math::Vector2 data) = 0;
            virtual bool VSetFloat3(std::string name, Math::Vector3 data) = 0;
            virtual bool VSetFloat4(std::string name, Math::Vector4 data) = 0;
            virtual bool VSetMatrix4(std::string name, Math::Matrix4 data) = 0;

            virtual bool VUpdate() = 0;
        protected:
            std::map<std::string, Resource::ShaderVariable*> m_shaderVariables;

            friend class Pipeline;
        };
    }
}