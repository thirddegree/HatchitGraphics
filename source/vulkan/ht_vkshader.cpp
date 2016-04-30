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
#include <ht_vkrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Resource;

            VKShader::VKShader(Core::Guid ID) :
                Core::RefCounted<VKShader>(std::move(ID)),
                m_shader(VK_NULL_HANDLE)
            {}

            bool VKShader::Initialize(const std::string& fileName, VKRenderer* renderer)
            {
                m_device = &(renderer->GetVKDevice()).GetVKDevices()[0];

                m_shader = VK_NULL_HANDLE;
                ShaderHandle shaderResourceHandle = Shader::GetHandleFromFileName(fileName);

                size_t size = shaderResourceHandle->GetBytecodeSize();
                const BYTE* shaderCode = shaderResourceHandle->GetBytecode();

                VkResult err;

                VkShaderModuleCreateInfo moduleCreateInfo = {};
                moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleCreateInfo.pNext = nullptr;
                moduleCreateInfo.codeSize = size;
                moduleCreateInfo.pCode = (uint32_t*)(shaderCode);
                moduleCreateInfo.flags = 0;

                err = vkCreateShaderModule(*m_device, &moduleCreateInfo, nullptr, &m_shader);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKShader::VInitFromFile(): Error creating shader module\n");
                    return false;
                }
                return true;
            }

            VKShader::~VKShader() 
            {
                if(m_shader != VK_NULL_HANDLE)
                    vkDestroyShaderModule(*m_device, m_shader, nullptr);
            }

            VkShaderModule VKShader::GetShaderModule()
            {
                return m_shader;
            }
        }
    }
}
