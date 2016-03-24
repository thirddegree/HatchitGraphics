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
#include <ht_vkrendertarget.h>
#include <ht_vkpipeline.h>
#include <ht_vkmaterial.h>
#include <ht_vkmesh.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKRenderPass::VKRenderPass(VkDevice& device, VkCommandPool& commandPool) :
                m_device(device), m_commandPool(commandPool)
            {
                m_device = device;
                m_commandPool = commandPool;

                m_commandBuffer = VK_NULL_HANDLE;
            }
            VKRenderPass::~VKRenderPass() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Destroy the command buffers
                if(m_commandBuffer != VK_NULL_HANDLE)
                    vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);

                //Destroy the render pass
                vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            }

            bool VKRenderPass::VPrepare()
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

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
                attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].flags = 0;

                VkAttachmentReference colorReference = {};
                colorReference.attachment = 0;
                colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthReference = {};
                depthReference.attachment = 1;
                depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass = {};
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

                VkRenderPassCreateInfo renderPassInfo = {};
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

                err = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
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

            ///Render the scene
            void VKRenderPass::VUpdate()
            {
                
            }

            bool VKRenderPass::VBuildCommandList() 
            {
                if (!allocateCommandBuffer())
                    return false;

                //Setup the order of the commands we will issue in the command list
                BuildRenderRequestHeirarchy();

                VkResult err;

                VKRenderer* renderer = VKRenderer::RendererInstance;
                VKRenderTarget* renderTarget = static_cast<VKRenderTarget*>(m_renderTarget);

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
                clearValues[0].color = m_clearColor.color;
                clearValues[1].depthStencil = { 1.0f, 0 };

                VkRenderPassBeginInfo renderPassBeginInfo = {};
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = m_renderPass;
                renderPassBeginInfo.framebuffer = renderTarget->GetVKFramebuffer();
                renderPassBeginInfo.renderArea.offset.x = 0;
                renderPassBeginInfo.renderArea.offset.y = 0;
                renderPassBeginInfo.renderArea.extent.width = m_width;
                renderPassBeginInfo.renderArea.extent.height = m_height;
                renderPassBeginInfo.clearValueCount = 2;
                renderPassBeginInfo.pClearValues = clearValues;

                VkViewport viewport = {};
                viewport.width = static_cast<float>(m_width);
                viewport.height = static_cast<float>(m_height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                VkRect2D scissor = {};
                scissor.extent.width = m_width;
                scissor.extent.height = m_height;
                scissor.offset.x = 0;
                scissor.offset.y = 0;
                
                err = vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderPass::VBuildCommandList(): Failed to build command buffer.\n");
#endif
                    return false;
                }

                /*
                    BEGIN BUFFER COMMANDS
                */

                vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

                std::map<IPipeline*, std::vector<Renderable>>::iterator iterator;

                for (iterator = m_pipelineList.begin(); iterator != m_pipelineList.end(); iterator++)
                {
                    VKPipeline* pipeline = static_cast<VKPipeline*>(iterator->first);

                    pipeline->VSetMatrix4("pass.proj", m_proj);
                    pipeline->VSetMatrix4("pass.view", m_view);
                    pipeline->VUpdate();

                    VkPipeline vkPipeline = pipeline->GetVKPipeline();
                    VkPipelineLayout vkPipelineLayout = pipeline->GetVKPipelineLayout();

                    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetVKPipeline());
                    pipeline->SendPushConstants(m_commandBuffer);

                    std::vector<Renderable> renderables = iterator->second;

                    VkDeviceSize offsets[] = { 0 };

                    for (uint32_t i = 0; i < renderables.size(); i++)
                    {
                        VKMaterial* material = static_cast<VKMaterial*>(renderables[i].material);
                        VKMesh*     mesh = static_cast<VKMesh*>(renderables[i].mesh);
                    
                        VkDescriptorSet* descriptorSet = material->GetVKDescriptorSet();
                        
                        vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vkPipelineLayout, 1, 1, descriptorSet, 0, nullptr);
                    
                        UniformBlock vertBlock = mesh->GetVertexBlock();
                        UniformBlock indexBlock = mesh->GetIndexBlock();
                        uint32_t indexCount = mesh->GetIndexCount();

                        vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &vertBlock.buffer, offsets);
                        vkCmdBindIndexBuffer(m_commandBuffer, indexBlock.buffer, 0, VK_INDEX_TYPE_UINT32);
                        vkCmdDrawIndexed(m_commandBuffer, indexCount, 1, 0, 0, 0);
                    }
                }

                vkCmdEndRenderPass(m_commandBuffer);

                /*
                    END BUFFER COMMANDS
                */

                //Blit to render target

                if (!renderTarget->Blit(m_commandBuffer))
                    return false;
                
                err = vkEndCommandBuffer(m_commandBuffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderPass::VBuildCommandList(): Failed to end command buffer.\n");
#endif
                    return false;
                }

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

            const VkRenderPass* VKRenderPass::GetVkRenderPass() { return &m_renderPass; }

            bool VKRenderPass::allocateCommandBuffer()
            {
                VkResult err;

                if (m_commandBuffer != VK_NULL_HANDLE)
                    return true;

                //Create internal command buffer
                VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
                cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                cmdBufferAllocInfo.commandPool = m_commandPool;
                cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                cmdBufferAllocInfo.commandBufferCount = 1;

                err = vkAllocateCommandBuffers(m_device, &cmdBufferAllocInfo, &m_commandBuffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareDescriptorLayout(): Failed to allocate command buffer\n");
#endif
                    return false;
                }

                return true;
            }
        }
    }
}
