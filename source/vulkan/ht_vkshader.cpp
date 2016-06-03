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

#include <ht_vkshader.h>
#include "spirv_glsl.hpp"

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Resource;

            VKShader::VKShader()
            {
                m_shader = VK_NULL_HANDLE;
            }

            bool VKShader::Initialize(Resource::ShaderHandle handle, const VkDevice& device)
            {
                m_device = device;

                size_t size = handle->GetBytecodeSize();
                const BYTE* shaderCode = handle->GetBytecode();

                //Convert shader bytecode to SPV ops which are uint32_ts
                //SPIRV-Cross expects a vector
                std::vector<uint32_t> shaderOps;
                size_t uintsize = (size / sizeof(uint32_t));
                shaderOps.resize(uintsize);
                memcpy(shaderOps.data(), shaderCode, size);

                VkResult err;

                VkShaderModuleCreateInfo moduleCreateInfo = {};
                moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleCreateInfo.pNext = nullptr;
                moduleCreateInfo.codeSize = size;
                moduleCreateInfo.pCode = (uint32_t*)(shaderCode);
                moduleCreateInfo.flags = 0;

                err = vkCreateShaderModule(m_device, &moduleCreateInfo, nullptr, &m_shader);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKShader::VInitFromFile(): Error creating shader module\n");
                    return false;
                }

                //TEST REFLECTION
                //spirv_cross::CompilerGLSL glsl(std::move(shaderOps));
                //spirv_cross::ShaderResources resources = glsl.get_shader_resources();
                //
                //// Get all sampled images in the shader.
                //HT_DEBUG_PRINTF("Shader Images: \n");
                //for (auto &resource : resources.storage_images)
                //{
                //    unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
                //    unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
                //    HT_DEBUG_PRINTF("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
                //
                //    // Modify the decoration to prepare it for GLSL.
                //    glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);
                //
                //    // Some arbitrary remapping if we want.
                //    glsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
                //}

                return true;
            }

            VKShader::~VKShader() 
            {
                if(m_shader != VK_NULL_HANDLE)
                    vkDestroyShaderModule(m_device, m_shader, nullptr);
            }

            VkShaderModule VKShader::GetShaderModule()
            {
                return m_shader;
            }
        }
    }
}
