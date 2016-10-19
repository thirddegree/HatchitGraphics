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
 * \file ht_vkshadermodule.h
 * \brief VKShaderModule class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#pragma once

#include <ht_platform.h>
#include <vulkan/vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKShaderModule
            {
            public:
                enum TypeOfShader
                {
                    Fragment,
                    Vertex
                };

                VKShaderModule();
                ~VKShaderModule();

                /* Compiled vertex or fragment shaders code */
                bool Load(TypeOfShader pType, const char* pCode, const uint32_t pSize);
                VkPipelineShaderStageCreateInfo GetStageCreateInfo() const;

                operator VkShaderModule();
                operator VkShaderModule*();

            private:
                VkShaderModule m_vkShader;
                VkDevice m_vkDevice;
                TypeOfShader m_Type;
            };
        }
    }
}
