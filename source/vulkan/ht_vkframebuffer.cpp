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
 * \file ht_vkframebuffer.h
 * \brief VkFrameBuffer class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vkframebuffer.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKFrameBuffer::VKFrameBuffer() : m_Buffer{VK_NULL_HANDLE}
            {
            }

            VKFrameBuffer::~VKFrameBuffer()
            {
                vkDestroyFramebuffer(m_Device, m_Buffer, nullptr);
            }

            bool VKFrameBuffer::Initialize(VKDevice &pDevice)
            {
                VkFramebufferCreateInfo pCreateInfo;

                pCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

                VkResult err = VK_SUCCESS;

                err = vkCreateFramebuffer(pDevice, &pCreateInfo, nullptr, &m_Buffer);
                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VkFrameBuffer::Initialize(): Failed to create framebuffer. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }
        }
    }
}
