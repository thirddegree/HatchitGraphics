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

#include <ht_vkshader.h>

namespace Hatchit {

    namespace Graphics {

        VKShader::VKShader() {}
        VKShader::~VKShader() {}

        void VKShader::VOnLoaded()
        {
        }
        void VKShader::VCompile()
        {
        }

        bool VKShader::VSetData(std::string name, const void* data, size_t size)
        { 
            return true;
        }
        bool VKShader::VSetInt(std::string name, int data)
        {
            return true;
        }
        bool VKShader::VSetFloat(std::string name, float data)
        {
            return true;
        }
        bool VKShader::VSetFloat2(std::string name, Math::Vector2 data)
        {
            return true;
        }
        bool VKShader::VSetFloat3(std::string name, Math::Vector3 data)
        {
            return true;
        }
        bool VKShader::VSetFloat4(std::string name, Math::Vector4 data)
        {
            return true;
        }
        bool VKShader::VSetMatrix3(std::string name, Math::Matrix3 data)
        {
            return true;
        }
        bool VKShader::VSetMatrix4(std::string name, Math::Matrix4 data)
        {
            return true;
        }

        bool VKShader::VBindTexture(std::string name, ITexture* texture)
        {
            return true;
        }
        bool VKShader::VUnbindTexture(std::string name, ITexture* texture)
        {
            return true;
        }

        VkShaderModule VKShader::GetShaderModule()
        {
            return m_shader;
        }
    }
}
