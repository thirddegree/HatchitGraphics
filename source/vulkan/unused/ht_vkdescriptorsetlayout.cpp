
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
 * \file ht_vkdescriptorsetlayout.cpp
 * \brief VKDescriptorSetLayout class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkdescriptorsetlayout.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKDescriptorSetLayout::VKDescriptorSetLayout() : m_Device{VK_NULL_HANDLE}, m_DescSetLayout{VK_NULL_HANDLE}
            {
            }

            VKDescriptorSetLayout::~VKDescriptorSetLayout()
            {
                vkDestroyDescriptorSetLayout(m_Device, m_DescSetLayout, nullptr);
            }

            bool VKDescriptorSetLayout::Initialize(VKDevice& pDevice, const VkDescriptorSetLayoutCreateInfo& pCreateInfo)
            {
                m_Device = pDevice;

                VkResult err = VK_SUCCESS;

                err = vkCreateDescriptorSetLayout(m_Device, &pCreateInfo, nullptr, &m_DescSetLayout);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKDescriptorSetLayout::Initialize(): Failed to create DescriptorSetLayout. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKDescriptorSetLayout::operator VkDescriptorSetLayout()
            {
                return m_DescSetLayout;
            }

            VKDescriptorSetLayout::operator VkDescriptorSetLayout*()
            {
                return &m_DescSetLayout;
            }
        }
    }
}
