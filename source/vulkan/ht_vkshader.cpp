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

            VKShader::VKShader(std::string fileName) : Resource::Resource(std::move(fileName)) {}
            VKShader::~VKShader() 
            {
                VkDevice device = VKRenderer::RendererInstance->GetVKDevice();

                vkDestroyShaderModule(device, m_shader, nullptr);
            }

            bool VKShader::VInitFromFile(const std::string& file)
            {
				Core::File f;
				f.Open(Core::os_exec_dir() + file, Core::FileMode::ReadBinary);


                VkDevice device = VKRenderer::RendererInstance->GetVKDevice();

                size_t size = f.SizeBytes();
                BYTE* shaderCode = new BYTE[size];
                f.Read(shaderCode, size);

                VkResult err;

                VkShaderModuleCreateInfo moduleCreateInfo = {};
                moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleCreateInfo.pNext = nullptr;
                moduleCreateInfo.codeSize = size;
                moduleCreateInfo.pCode = (uint32_t*)shaderCode;
                moduleCreateInfo.flags = 0;

                err = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &m_shader);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKShader::VInitFromFile(): Error creating shader module\n");
#endif
                    return false;
                }

                return true;
            }

            VkShaderModule VKShader::GetShaderModule()
            {
                return m_shader;
            }

			VKShader* VKShader::GetRawPointer()
			{
				return this;
			}
        }
    }
}
