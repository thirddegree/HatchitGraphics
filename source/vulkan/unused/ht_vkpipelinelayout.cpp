
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
 * \file ht_vkpipelinelayout.cpp
 * \brief VKPipelineLayout class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkpipelinelayout.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKPipelineLayout::VKPipelineLayout() : m_Device{VK_NULL_HANDLE}, m_PipelineLayout{VK_NULL_HANDLE}
            {
            }

            VKPipelineLayout::~VKPipelineLayout()
            {
                vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
            }

            bool VKPipelineLayout::Initialize(VKDevice& pDevice, const VkPipelineLayoutCreateInfo& pCreateInfo)
            {
                m_Device = pDevice;

                VkResult err = VK_SUCCESS;

                err = vkCreatePipelineLayout(m_Device, &pCreateInfo, nullptr, &m_PipelineLayout);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKPipelineLayout::Initialize(): Failed to create pipeline. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKPipelineLayout::operator VkPipelineLayout()
            {
                return m_PipelineLayout;
            }

            VKPipelineLayout::operator VkPipelineLayout*()
            {
                return &m_PipelineLayout;
            }

        }
    }
}
