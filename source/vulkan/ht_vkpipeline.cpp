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
 * \file ht_vkpipeline.cpp
 * \brief VkPipeline class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkpipeline.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKPipeline::VKPipeline(VKDevice& pDevice) : m_PipelineLayout{VK_NULL_HANDLE}, m_Device{pDevice}
            {
            }

            VKPipeline::~VKPipeline()
            {
                vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
            }

            bool VKPipeline::Initialize(const VkPipelineLayoutCreateInfo& pCreateInfo)
            {
                VkResult err = VK_SUCCESS;

                err = vkCreatePipelineLayout(m_Device, &pCreateInfo, nullptr, &m_PipelineLayout);

                if ( err != VK_SUCCESS)
                {
                        HT_ERROR_PRINTF("VKPipeline::Initialize(): Failed to create pipeline. %s\n", VKErrorString(err));
                        return false;
                }

                return true;
            }

            VKPipeline::operator VkPipelineLayout()
            {
                return m_PipelineLayout;
            }

            VKPipeline::operator VkPipelineLayout*()
            {
                return &m_PipelineLayout;
            }
        }
    }
}
