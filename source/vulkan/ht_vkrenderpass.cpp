
/**
 **    Hatchit Engine
 **    Copyright(c) 2015-2017 Third-Degree
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
 * @file ht_vkrenderpass.cpp
 * @brief VKRenderPass class implementation
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 */

#include <ht_vkrenderpass.h>
#include <ht_vkdevice.h>
#include <ht_vkswapchain.h>
#include <array>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKRenderPass::VKRenderPass():
                m_vkRenderPass(VK_NULL_HANDLE),
                m_vkDevice(VK_NULL_HANDLE)
            {
            }

            VKRenderPass::~VKRenderPass()
            {
                if(m_vkRenderPass != VK_NULL_HANDLE)
                {
                    vkDestroyRenderPass(m_vkDevice, m_vkRenderPass, nullptr);
                    m_vkRenderPass = VK_NULL_HANDLE;
                }
                
            }

            bool VKRenderPass::Initialize(VKDevice& device, VKSwapChain& swapchain)
            {
                std::array<VkAttachmentDescription, 2> attachments;

                //Color attachment
                attachments[0].format = swapchain.GetSurfaceFormat();
                attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                attachments[0].flags = 0;

                //Depth attachment
                device.GetSupportedDepthFormat(attachments[1].format);
                attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].flags = 0;

                //Setup attachment references
                VkAttachmentReference colorAttachmentRef;
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthAttachmentRef;
                depthAttachmentRef.attachment = 1;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                //Setup subpass reference description
                VkSubpassDescription subpassDesc;
                subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpassDesc.colorAttachmentCount = 1;
                subpassDesc.pColorAttachments = &colorAttachmentRef;
                subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;
                subpassDesc.inputAttachmentCount = 0;
                subpassDesc.pInputAttachments = nullptr;
                subpassDesc.preserveAttachmentCount = 0;
                subpassDesc.pPreserveAttachments = nullptr;
                subpassDesc.pResolveAttachments = nullptr;
                subpassDesc.flags = 0;

                //Setup subpass dependencies
                std::array<VkSubpassDependency, 2> dependencies;

                // Dependency [0] : Renderpass Start
                // Handles transition from final to initial
                // layout
                dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
                dependencies[0].dstSubpass = 0;
                dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                // Dependency [1] : Renderpass End
                // Handles transition from initial to final layout
                dependencies[1].srcSubpass = 0;
                dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
                dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                // Create renderpass
                VkRenderPassCreateInfo passInfo;
                passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                passInfo.pNext = nullptr;
                passInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                passInfo.pAttachments = attachments.data();
                passInfo.subpassCount = 1;
                passInfo.pSubpasses = &subpassDesc;
                passInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
                passInfo.pDependencies = dependencies.data();
                passInfo.flags = 0;

                return Initialize(device, passInfo);
            }

            bool VKRenderPass::Initialize(VKDevice& device, const VkRenderPassCreateInfo& info)
            {
                m_vkDevice = static_cast<VkDevice>(device);

                VkResult err = VK_SUCCESS;

                err = vkCreateRenderPass(m_vkDevice, &info, nullptr, &m_vkRenderPass);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKRenderPass::Initialize(): Failed to create renderpass. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKRenderPass::operator VkRenderPass() const
            {
                return m_vkRenderPass;
            }

            VKRenderPass::operator VkRenderPass*()
            {
                return &m_vkRenderPass;
            }

            
        }
    }
}
