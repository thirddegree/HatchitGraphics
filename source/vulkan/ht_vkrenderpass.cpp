/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
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

#include <ht_vkrenderpass.h>
#include <ht_vkrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKRenderPass::VKRenderPass() {}
            VKRenderPass::~VKRenderPass() {}

            bool VKRenderPass::VPrepare()
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                VkDevice device = renderer->GetVKDevice();

                //Setup render pass

                VkAttachmentDescription attachments[2];
                attachments[0].format = renderer->GetPreferredImageFormat();
                attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments[0].flags = 0;

                attachments[1].format = renderer->GetPreferredDepthFormat();
                attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].flags = 0;

                VkAttachmentReference colorReference;
                colorReference.attachment = 0;
                colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthReference;
                depthReference.attachment = 1;
                depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass;
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.flags = 0;
                subpass.inputAttachmentCount = 0;
                subpass.pInputAttachments = nullptr;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorReference;
                subpass.pResolveAttachments = nullptr;
                subpass.pDepthStencilAttachment = &depthReference;
                subpass.preserveAttachmentCount = 0;
                subpass.pPreserveAttachments = nullptr;

                VkRenderPassCreateInfo renderPassInfo;
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.pNext = nullptr;
                renderPassInfo.attachmentCount = 2;
                renderPassInfo.pAttachments = attachments;
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpass;
                renderPassInfo.dependencyCount = 0;
                renderPassInfo.pDependencies = nullptr;
                renderPassInfo.flags = 0;

                VkResult err;

                err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareDescriptorLayout(): Failed to create render pass\n");
#endif
                    return false;
                }

                return true;
            }

            //Will this be sent the Objects that it needs to render?
            ///Render the scene
            void VKRenderPass::VRender()
            {
            }

            void VKRenderPass::VSetRenderTarget(IRenderTarget* renderTarget)
            {
                m_renderTarget = renderTarget;
            }

            bool VKRenderPass::VBuildCommandList() 
            {
                VkResult err;

                VkCommandBufferInheritanceInfo inheritanceInfo = {};
                inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
                inheritanceInfo.pNext = nullptr;
                inheritanceInfo.renderPass = VK_NULL_HANDLE;
                inheritanceInfo.subpass = 0;
                inheritanceInfo.framebuffer = VK_NULL_HANDLE;
                inheritanceInfo.occlusionQueryEnable = VK_FALSE;
                inheritanceInfo.queryFlags = 0;
                inheritanceInfo.pipelineStatistics = 0;

                VkCommandBufferBeginInfo beginInfo = {};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.pNext = nullptr;
                beginInfo.flags = 0;
                beginInfo.pInheritanceInfo = &inheritanceInfo;

                VkClearValue clearValues[2] = {};
                clearValues[0] = m_clearColor;
                clearValues[1] = {1.0f, 0.0f};

                /*
                VkRenderPassBeginInfo renderPassBeginInfo = {};
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = m_renderPass;
                renderPassBeginInfo.framebuffer = m_framebuffers[0];
                renderPassBeginInfo.renderArea.offset.x = 0;
                renderPassBeginInfo.renderArea.offset.y = 0;
                renderPassBeginInfo.renderArea.extent.width = m_width;
                renderPassBeginInfo.renderArea.extent.height = m_height;
                renderPassBeginInfo.clearValueCount = 2;
                renderPassBeginInfo.pClearValues = clearValues;

                err = vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderPass::VBuildCommandList(): Failed to build command buffer.\n");
#endif
                    return false;
                }

                vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, );
                vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);

                VkViewport viewport = {};
                viewport.width = static_cast<float>(m_width);
                viewport.height = static_cast<float>(m_height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

                VkRect2D scissor = {};
                scissor.extent.width = m_width;
                scissor.extent.height = m_height;
                scissor.offset.x = 0;
                scissor.offset.y = 0;
                vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

                vkCmdDraw(m_commandBuffer, 0, 0, 0, 0);
                vkCmdEndRenderPass(m_commandBuffer);

                VkImageMemoryBarrier prePresentBarrier = {};
                prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                prePresentBarrier.pNext = nullptr;
                prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

                prePresentBarrier.image = m_swapchainBuffers[0].image;

                vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

                err = vkEndCommandBuffer(m_commandBuffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderPass::VBuildCommandList(): Failed to end command buffer.\n");
#endif
                    return false;
                }
                */
                return true;
            }

            void VKRenderPass::VSetClearColor(Color color)
            {
                m_clearColor.color.float32[0] = color.r;
                m_clearColor.color.float32[1] = color.g;
                m_clearColor.color.float32[2] = color.b;
                m_clearColor.color.float32[3] = color.a;
            }

            VkCommandBuffer VKRenderPass::GetVkCommandBuffer() { return m_commandBuffer; }

            VkRenderPass VKRenderPass::GetVkRenderPass() { return m_renderPass; }
        }
    }
}
