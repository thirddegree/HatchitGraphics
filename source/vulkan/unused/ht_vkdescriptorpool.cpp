
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
 * \file ht_vkdescriptorpool.cpp
 * \brief VKDescriptorPool class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkdescriptorpool.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKDescriptorPool::VKDescriptorPool() : m_Device{VK_NULL_HANDLE}, m_DescriptorPool{VK_NULL_HANDLE}
            {
            }

            VKDescriptorPool::~VKDescriptorPool()
            {
                vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
            }

            bool VKDescriptorPool::Initialize(Hatchit::Graphics::Vulkan::VKDevice &pDevice, const VkDescriptorPoolCreateInfo &pCreateInfo)
            {
                m_Device = pDevice;

                VkResult err = VK_SUCCESS;

                err = vkCreateDescriptorPool(m_Device, &pCreateInfo, nullptr, &m_DescriptorPool);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKDescriptorPool::Initialize(): Failed to create descriptor pool. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKDescriptorPool::operator VkDescriptorPool()
            {
                return m_DescriptorPool;
            }

            VKDescriptorPool::operator VkDescriptorPool*()
            {
                return &m_DescriptorPool;
            }
        }
    }
}
