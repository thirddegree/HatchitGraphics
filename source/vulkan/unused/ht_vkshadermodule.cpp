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
 * \file ht_vkshadermodule.cpp
 * \brief VKShaderModule class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkshadermodule.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKShaderModule::VKShaderModule() : m_vkShader{VK_NULL_HANDLE}, m_vkDevice{VK_NULL_HANDLE}
            {
            }

            VKShaderModule::~VKShaderModule()
            {
                vkDestroyShaderModule(m_vkDevice, m_vkShader, nullptr);
            }

            bool VKShaderModule::Load(TypeOfShader pType, const char *pCode, const uint32_t pSize)
            {
                m_Type = pType;

                VkShaderModuleCreateInfo pCreateInfo;

                pCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                pCreateInfo.codeSize = pSize;
                pCreateInfo.pCode = const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(&pCode));

                VkResult err = VK_SUCCESS;

                err = vkCreateShaderModule(m_vkDevice, &pCreateInfo, nullptr, &m_vkShader);
                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VkShaderModule::Load(): Failed to create shader module");
                    return false;
                }

                return true;
            }

            VkPipelineShaderStageCreateInfo VKShaderModule::GetStageCreateInfo() const
            {
                VkPipelineShaderStageCreateInfo pShaderStageInfo;
                pShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

                switch (m_Type)
                {
                case Fragment:
                    pShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;
                case Vertex:
                    pShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                }

                pShaderStageInfo.module = m_vkShader;

                /* Entry method should be set main as a pattern? */
                pShaderStageInfo.pName = "main";

                return pShaderStageInfo;
            }

            VKShaderModule::operator VkShaderModule()
            {
                return m_vkShader;
            }

            VKShaderModule::operator VkShaderModule*()
            {
                return &m_vkShader;
            }
        }
    }
}
