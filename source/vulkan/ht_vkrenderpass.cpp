
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
 * \file ht_vkrenderpass.cpp
 * \brief VKRenderPass class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkrenderpass.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKRenderPass::VKRenderPass() : m_Device{VK_NULL_HANDLE}, m_RenderPass{VK_NULL_HANDLE}
            {
            }

            VKRenderPass::~VKRenderPass()
            {
                vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
            }

            bool VKRenderPass::Initialize(VKDevice& pDevice, const VkRenderPassCreateInfo& pCreateInfo)
            {
                m_Device = pDevice;

                VkResult err = VK_SUCCESS;

                err = vkCreateRenderPass(m_Device, &pCreateInfo, nullptr, &m_RenderPass);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKRenderPass::Initialize(): Failed to create renderpass. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKRenderPass::operator VkRenderPass()
            {
                return m_RenderPass;
            }

            VKRenderPass::operator VkRenderPass*()
            {
                return &m_RenderPass;
            }
        }
    }
}
