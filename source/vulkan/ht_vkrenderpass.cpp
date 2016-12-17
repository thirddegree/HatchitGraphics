
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
            VKRenderPass::VKRenderPass(uint64_t id)
                : Hatchit::Resource::FileResource<VKRenderPass>(id)
            {
                m_device = VK_NULL_HANDLE;
                m_renderpass = VK_NULL_HANDLE;
            }

            VKRenderPass::~VKRenderPass()
            {
                vkDestroyRenderPass(m_device, m_renderpass, nullptr);
            }

            bool VKRenderPass::Initialize(VKDevice& device, const VkRenderPassCreateInfo& info)
            {
                /**
                * Initializing this resource will require
                * a handle to a RenderPass file resource which describes the various inputs
                * and outputs of a renderpass.
                * 
                * The outputs and input of a renderpass are rendertarget objects. A renderpass
                * is one step in potentially a multipass or "deferred" renderer. Exposing
                * this concept through files would allow the user to control their rendering
                * pipeline at a more granular level than hiding it internally.
                *
                * At least, that is what the original design philosophy is trying to achieve...
                */
                

                m_device = device;

                VkResult err = VK_SUCCESS;

                err = vkCreateRenderPass(m_device, &info, nullptr, &m_renderpass);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKRenderPass::Initialize(): Failed to create renderpass. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKRenderPass::operator VkRenderPass()
            {
                return m_renderpass;
            }

            VKRenderPass::operator VkRenderPass*()
            {
                return &m_renderpass;
            }
        }
    }
}
