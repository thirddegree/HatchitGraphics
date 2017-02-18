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
            VKPipeline::VKPipeline()
                : m_Pipeline{VK_NULL_HANDLE}, m_Device{VK_NULL_HANDLE}, m_PipelineCache{VK_NULL_HANDLE}
            {
            }

            VKPipeline::~VKPipeline()
            {
                vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
            }

            bool VKPipeline::Initialize(Hatchit::Graphics::Vulkan::VKDevice &pDevice, Hatchit::Graphics::Vulkan::VKPipelineCache &pPipelineCache, const VkGraphicsPipelineCreateInfo &pCreateInfo)
            {
                m_Device = pDevice;
                m_PipelineCache = pPipelineCache;

                VkResult err = VK_SUCCESS;

                err = vkCreateGraphicsPipelines(m_Device, m_PipelineCache, 1, &pCreateInfo, nullptr, &m_Pipeline);

                if ( err != VK_SUCCESS)
                {
                        HT_ERROR_PRINTF("VKPipeline::Initialize(): Failed to create pipeline. %s\n", VKErrorString(err));
                        return false;
                }

                return true;
            }

            VKPipeline::operator VkPipeline()
            {
                return m_Pipeline;
            }

            VKPipeline::operator VkPipeline*()
            {
                return &m_Pipeline;
            }
        }
    }
}
