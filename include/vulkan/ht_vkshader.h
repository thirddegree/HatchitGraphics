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
* \class IShader
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will load a shader with Vulkan
*
* A resource object that after loading GLSL code or SPIR-V ops to a 
* void* will act as an interface to the shader object.
*/

#pragma once

#include <ht_vulkan.h>
#include <ht_shader.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKShader : public ShaderBase
            {
                friend class VKSwapChain;
            public:
                VKShader(Core::Guid ID);
                VKShader(VKShader&&) = default;
                ~VKShader();

                //Required function for RefCounted classes
                bool Initialize(const std::string& fileName, VKRenderer* renderer);

                VkShaderModule GetShaderModule();

            private:
                const VkDevice* m_device;
                VkShaderModule m_shader;
            };

        }
    }
}
