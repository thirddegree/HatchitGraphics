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

            VKRenderPass::VKRenderPass(std::string ID) :
                m_device(VKRenderer::RendererInstance->GetVKDevice()), 
                m_commandPool(VKRenderer::RendererInstance->GetVKCommandPool()),
                Core::RefCounted<VKRenderPass>(std::move(ID)),
                m_commandBuffer(VK_NULL_HANDLE),
                m_renderPass(VK_NULL_HANDLE)
            {}

            bool VKRenderPass::Initialize(const std::string& fileName)
            {
                //Load resources
                m_renderPassResourceHandle = Resource::RenderPass::GetHandleFromFileName(fileName);
                
                if (m_renderPassResourceHandle.IsValid())
                {
                    std::vector<std::string> inputPaths = m_renderPassResourceHandle->GetInputPaths();
                    std::vector<std::string> outputPaths = m_renderPassResourceHandle->GetOutputPaths();

                    for (size_t i = 0; i < inputPaths.size(); i++)
                    {
                        IRenderTargetHandle inputTargetHandle = VKRenderTarget::GetHandle(inputPaths[i], inputPaths[i]).StaticCastHandle<IRenderTarget>();
                        m_inputRenderTargets.push_back(inputTargetHandle);
                    }

                    for (size_t i = 0; i < outputPaths.size(); i++)
                    {
                        IRenderTargetHandle outputTargetHandle = VKRenderTarget::GetHandle(outputPaths[i], outputPaths[i]).StaticCastHandle<IRenderTarget>();
                        m_outputRenderTargets.push_back(outputTargetHandle);
                    }

                    if(!VPrepare())
                    {
                        HT_DEBUG_PRINTF("Error: Tried to load VKRenderPass but preperation failed!\n");
                        return false;
                    }
                    return true;
                }
                else
                {
                    HT_DEBUG_PRINTF("Error: Tried to load VKRenderPass but the resource handle was invalid!\n");
                    return false;
                }
            }
            VKRenderPass::~VKRenderPass() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Command buffer will be freed with command pool
                //if(m_commandBuffer != VK_NULL_HANDLE)
                //    vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);

                //Destroy the render pass
                if(m_renderPass != VK_NULL_HANDLE)
                    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            }

            bool VKRenderPass::VPrepare()
            {
                if (!setupRenderPass())
                    return false;
                if (!setupAttachmentImages())
                    return false;
                if (!setupFramebuffer())
                    return false;

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

                //Get the current clear color from the renderer
                VkClearValue clearColor = renderer->GetClearColor();

                VkClearValue clearValues[2] = {};
                clearValues[0] = clearColor;
                clearValues[1].depthStencil = { 1.0f, 0 };

                VkRenderPassBeginInfo renderPassBeginInfo = {};
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = m_renderPass;
                renderPassBeginInfo.framebuffer = m_framebuffer;
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
                    HT_DEBUG_PRINTF("VKRenderPass::VBuildCommandList(): Failed to build command buffer.\n");
                    return false;
                }

                /*
                    BEGIN BUFFER COMMANDS
                */

                vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

                std::map<IPipelineHandle, std::vector<Renderable>>::iterator iterator;

                for (iterator = m_pipelineList.begin(); iterator != m_pipelineList.end(); iterator++)
                {
                    VKPipelineHandle pipeline = iterator->first.DynamicCastHandle<VKPipeline>();

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
                        VKMaterialHandle material = renderables[i].material.DynamicCastHandle<VKMaterial>();
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

                //Blit to render targets
                for (size_t i = 0; i < m_outputRenderTargets.size(); i++)
                {
                    VKRenderTargetHandle renderTarget = m_outputRenderTargets[i].DynamicCastHandle<VKRenderTarget>();

                    if (!renderTarget->Blit(m_commandBuffer, m_colorImages[i]))
                        return false;
                }
                
                err = vkEndCommandBuffer(m_commandBuffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderPass::VBuildCommandList(): Failed to end command buffer.\n");
                    return false;
                }

                return true;
            }

            VkCommandBuffer VKRenderPass::GetVkCommandBuffer() { return m_commandBuffer; }

            const VkRenderPass& VKRenderPass::GetVkRenderPass() { return m_renderPass; }

            /*
                Private methods
            */

            bool VKRenderPass::setupRenderPass() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Setup render pass

                std::vector<VkAttachmentDescription> attachmentDescriptions;
                std::vector<VkAttachmentReference> attachmentReferences;

                for (size_t i = 0; i < m_outputRenderTargets.size(); i++)
                {
                    VkAttachmentDescription description;

                    VKRenderTargetHandle m_vkOutputTarget = m_outputRenderTargets[i].DynamicCastHandle<VKRenderTarget>();

                    description.format = m_vkOutputTarget->GetVKColorFormat();
                    description.samples = VK_SAMPLE_COUNT_1_BIT;
                    description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                    description.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    description.flags = 0;

                    VkAttachmentReference reference;
                    reference.attachment = static_cast<uint32_t>(i);
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                    attachmentDescriptions.push_back(description);
                    attachmentReferences.push_back(reference);
                }

                VkAttachmentDescription depthAttachment;

                depthAttachment.format = renderer->GetPreferredDepthFormat();
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                depthAttachment.flags = 0;

                attachmentDescriptions.push_back(depthAttachment);

                VkAttachmentReference depthReference = {};
                depthReference.attachment = 1;
                depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass = {};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.flags = 0;
                subpass.inputAttachmentCount = 0;
                subpass.pInputAttachments = nullptr;
                subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentReferences.size());
                subpass.pColorAttachments = attachmentReferences.data();
                subpass.pResolveAttachments = nullptr;
                subpass.pDepthStencilAttachment = &depthReference;
                subpass.preserveAttachmentCount = 0;
                subpass.pPreserveAttachments = nullptr;

                VkRenderPassCreateInfo renderPassInfo = {};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.pNext = nullptr;
                renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
                renderPassInfo.pAttachments = attachmentDescriptions.data();
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
                    HT_DEBUG_PRINTF("VKRenderPass::setupRenderPass(): Failed to create render pass\n");
                    return false;
                }

                return true;
            }
            bool VKRenderPass::setupAttachmentImages() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                VkFormat depthFormat = renderer->GetPreferredDepthFormat();
                renderer->CreateSetupCommandBuffer();
                VkCommandBuffer setupCommand = renderer->GetSetupCommandBuffer();

                VkResult err;

                //If width and height were not set, lets use the size of the screen that the renderer reports
                if (m_width == 0)
                    m_width = renderer->GetWidth();
                if (m_height == 0)
                    m_height = renderer->GetHeight();

                //Create an image for every output texture
                for (size_t i = 0; i < m_outputRenderTargets.size(); i++)
                {
                    VKRenderTargetHandle vkRenderTarget = m_outputRenderTargets[i].DynamicCastHandle<VKRenderTarget>();
                    VkFormat colorFormat = vkRenderTarget->GetVKColorFormat();

                    //Attachment image that we will push back into a vector
                    Image colorImage;

                    uint32_t width = vkRenderTarget->GetWidth();
                    uint32_t height = vkRenderTarget->GetHeight();
                    if (width == 0)
                        width = m_width;
                    if (height == 0)
                        height = m_height;

                    //Color attachment
                    VkImageCreateInfo imageInfo = {};
                    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                    imageInfo.pNext = nullptr;
                    imageInfo.format = colorFormat;
                    imageInfo.imageType = VK_IMAGE_TYPE_2D;
                    imageInfo.extent = { width, height, 1 };
                    imageInfo.mipLevels = 1;
                    imageInfo.arrayLayers = 1;
                    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                    imageInfo.flags = 0;

                    VkMemoryAllocateInfo memAllocInfo = {};
                    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

                    VkMemoryRequirements memReqs;

                    err = vkCreateImage(m_device, &imageInfo, nullptr, &colorImage.image);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating color image!\n");
                        return false;
                    }

                    vkGetImageMemoryRequirements(m_device, colorImage.image, &memReqs);
                    memAllocInfo.allocationSize = memReqs.size;
                    renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

                    err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &colorImage.memory);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error allocating color image memory!\n");
                        return false;
                    }

                    err = vkBindImageMemory(m_device, colorImage.image, colorImage.memory, 0);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error binding color image memory!\n");
                        return false;
                    }

                    renderer->SetImageLayout(setupCommand, colorImage.image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                    VkImageViewCreateInfo viewInfo = {};
                    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    viewInfo.pNext = nullptr;
                    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    viewInfo.format = colorFormat;
                    viewInfo.flags = 0;
                    viewInfo.subresourceRange = {};
                    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    viewInfo.subresourceRange.baseMipLevel = 0;
                    viewInfo.subresourceRange.levelCount = 1;
                    viewInfo.subresourceRange.baseArrayLayer = 0;
                    viewInfo.subresourceRange.layerCount = 1;
                    viewInfo.image = colorImage.image;

                    err = vkCreateImageView(m_device, &viewInfo, nullptr, &colorImage.view);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating color image view!\n");
                        return false;
                    }

                    m_colorImages.push_back(colorImage);
                }

                //Create depth buffer
                VkImageCreateInfo imageInfo = {};
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.pNext = nullptr;
                imageInfo.format = depthFormat;
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent = { m_width, m_height, 1 };
                imageInfo.mipLevels = 1;
                imageInfo.arrayLayers = 1;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                imageInfo.flags = 0;

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

                VkMemoryRequirements memReqs;

                err = vkCreateImage(m_device, &imageInfo, nullptr, &m_depthImage.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating color image!\n");
                    return false;
                }

                vkGetImageMemoryRequirements(m_device, m_depthImage.image, &memReqs);
                memAllocInfo.allocationSize = memReqs.size;
                renderer->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_depthImage.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error allocating color image memory!\n");
                    return false;
                }

                err = vkBindImageMemory(m_device, m_depthImage.image, m_depthImage.memory, 0);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error binding color image memory!\n");
                    return false;
                }

                renderer->SetImageLayout(setupCommand, m_depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.pNext = nullptr;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = depthFormat;
                viewInfo.flags = 0;
                viewInfo.subresourceRange = {};
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;
                viewInfo.image = m_depthImage.image;

                err = vkCreateImageView(m_device, &viewInfo, nullptr, &m_depthImage.view);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating color image view!\n");
                    return false;
                }

                renderer->FlushSetupCommandBuffer();

                return true;
            }
            bool VKRenderPass::setupFramebuffer() 
            {
                VkResult err;

                //Create internal framebuffer
                std::vector<VkImageView> attachmentViews;

                for (size_t i = 0; i < m_colorImages.size(); i++)
                    attachmentViews.push_back(m_colorImages[i].view);
                attachmentViews.push_back(m_depthImage.view);

                VkFramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.flags = 0;
                framebufferInfo.renderPass = m_renderPass;
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
                framebufferInfo.pAttachments = attachmentViews.data();
                framebufferInfo.width = m_width;
                framebufferInfo.height = m_height;
                framebufferInfo.layers = 1;

                err = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderTarget::VPrepare(): Error creating framebuffer!\n");
                    return false;
                }

                return true;
            }

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
                    HT_DEBUG_PRINTF("VKRenderer::prepareDescriptorLayout(): Failed to allocate command buffer\n");
                    return false;
                }

                return true;
            }
        }
    }
}
