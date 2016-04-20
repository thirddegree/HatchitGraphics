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

            VKRenderPass::VKRenderPass(Core::Guid ID) :
                m_device(VKRenderer::RendererInstance->GetVKDevice()),
                m_commandPool(VKRenderer::RendererInstance->GetVKCommandPool()),
                m_descriptorPool(VKRenderer::RendererInstance->GetVKDescriptorPool()),
                Core::RefCounted<VKRenderPass>(std::move(ID))
            {
                m_width = 0;
                m_height = 0;

                m_view = Math::Matrix4();
                m_proj = Math::Matrix4();

                m_instanceData = nullptr;
                m_instanceDataSize = 0;
                m_currentInstanceDataOffset = 0;
                m_instanceChunkSize = 0;

                m_instanceBlock.buffer = VK_NULL_HANDLE;
                m_instanceBlock.memory = VK_NULL_HANDLE;

                m_commandBuffer = VK_NULL_HANDLE;
            }

            VKRenderPass::~VKRenderPass() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Free input descriptor sets
                vkFreeDescriptorSets(m_device, m_descriptorPool, static_cast<uint32_t>(m_inputTargetDescriptorSets.size()), m_inputTargetDescriptorSets.data());

                //Destroy framebuffer images
                for (size_t i = 0; i < m_colorImages.size(); i++)
                {
                    Image_vk image = m_colorImages[i];

                    vkDestroyImageView(m_device, image.view, nullptr);
                    vkDestroyImage(m_device, image.image, nullptr);
                    vkFreeMemory(m_device, image.memory, nullptr);
                }

                //Destroy depth image
                vkDestroyImageView(m_device, m_depthImage.view, nullptr);
                vkDestroyImage(m_device, m_depthImage.image, nullptr);
                vkFreeMemory(m_device, m_depthImage.memory, nullptr);
                
                //Free instance texel buffers
                if(m_instanceBlock.buffer != VK_NULL_HANDLE)
                    DeleteUniformBuffer(m_device, m_instanceBlock);

                //Destroy framebuffer
                vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);

                //Destroy the render pass
                vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            }

            bool VKRenderPass::Initialize(const std::string& fileName)
            {
                ////Load resources
                m_renderPassResourceHandle = Resource::RenderPass::GetHandleFromFileName(fileName);

                if (!m_renderPassResourceHandle.IsValid())
                {
                    HT_DEBUG_PRINTF("Error: Tried to load VKRenderPass but the resource handle was invalid!\n");
                    return false;
                }
                
                std::vector<Resource::RenderPass::InputTarget> inputTargets = m_renderPassResourceHandle->GetInputTargets();
                std::vector<std::string> outputPaths = m_renderPassResourceHandle->GetOutputPaths();

                //Create a structure to map set index to maps of binding indicies and render target handles
                std::map<uint32_t, std::map<uint32_t, VKRenderTargetHandle>> mappedInputTargets;

                for (size_t i = 0; i < inputTargets.size(); i++)
                {
                    std::string targetPath = inputTargets[i].path;
                    uint32_t targetSetIndex = inputTargets[i].set;
                    uint32_t targetBindingIndex = inputTargets[i].binding;

                    VKRenderTargetHandle inputTargetHandle = VKRenderTarget::GetHandle(targetPath, targetPath);

                    mappedInputTargets[targetSetIndex][targetBindingIndex] = inputTargetHandle;
                }

                for (size_t i = 0; i < outputPaths.size(); i++)
                {
                    IRenderTargetHandle outputTargetHandle = VKRenderTarget::GetHandle(outputPaths[i], outputPaths[i]).StaticCastHandle<IRenderTarget>();
                    m_outputRenderTargets.push_back(outputTargetHandle);
                }

                if (!setupAttachmentImages())
                    return false;
                if (!setupRenderPass())
                    return false;
                if (!setupFramebuffer())
                    return false;
                if (!setupDescriptorSets(mappedInputTargets))
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

                if (m_instanceBlock.buffer != VK_NULL_HANDLE)
                    DeleteUniformBuffer(m_device, m_instanceBlock);

                //Create block of data for instance variables
                if (m_instanceDataSize > 0)
                {
                    if (!CreateUniformBuffer(m_device, m_instanceDataSize, m_instanceData, &m_instanceBlock))
                        return false;
                }

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

                std::vector<VkClearValue> clearValues;
                for (size_t i = 0; i < m_outputRenderTargets.size(); i++)
                {
                    VKRenderTargetHandle vkTarget= m_outputRenderTargets[i].DynamicCastHandle<VKRenderTarget>();

                    const VkClearValue* targetClearColor = vkTarget->GetClearColor();
                    //If a clear color is provided by the render target, lets use that
                    if (targetClearColor == nullptr)
                        clearValues.push_back(clearColor);
                    else
                        clearValues.push_back(*targetClearColor);
                }
                clearValues.push_back({1.0f, 0.0f});

                VkRenderPassBeginInfo renderPassBeginInfo = {};
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = m_renderPass;
                renderPassBeginInfo.framebuffer = m_framebuffer;
                renderPassBeginInfo.renderArea.offset.x = 0;
                renderPassBeginInfo.renderArea.offset.y = 0;
                renderPassBeginInfo.renderArea.extent.width = m_width;
                renderPassBeginInfo.renderArea.extent.height = m_height;
                renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
                renderPassBeginInfo.pClearValues = clearValues.data();

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

                std::map<IPipelineHandle, std::vector<RenderableInstances>>::iterator iterator;

                for (iterator = m_pipelineList.begin(); iterator != m_pipelineList.end(); iterator++)
                {
                    VKPipelineHandle pipeline = iterator->first.DynamicCastHandle<VKPipeline>();

                    //Calculate inverse view proj
                    Math::Matrix4 invViewProj = Math::MMMatrixTranspose(Math::MMMatrixInverse(m_view));

                    m_view = Math::MMMatrixTranspose(m_view);
                    m_proj = Math::MMMatrixTranspose(m_proj);

                    pipeline->VSetMatrix4("pass.0proj", m_proj);
                    pipeline->VSetMatrix4("pass.1view", m_view);
                    pipeline->VSetMatrix4("pass.2invViewProj", invViewProj);
                    pipeline->VSetInt("pass.3width", m_width);
                    pipeline->VSetInt("pass.4height", m_height);
                    pipeline->VUpdate();

                    VkPipeline vkPipeline = pipeline->GetVKPipeline();
                    VkPipelineLayout vkPipelineLayout = renderer->GetVKRootLayoutHandle()->VKGetPipelineLayout();

                    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
                    pipeline->SendPushConstants(m_commandBuffer, vkPipelineLayout);

                    //Bind input textures
                    if(m_inputTargetDescriptorSets.size() > 0)
                        vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, m_firstInputTargetSetIndex,
                            static_cast<uint32_t>(m_inputTargetDescriptorSets.size()), m_inputTargetDescriptorSets.data(), 0, nullptr);

                    std::vector<RenderableInstances> renderables = iterator->second;

                    VkDeviceSize offsets[] = { 0 };

                    for (uint32_t i = 0; i < renderables.size(); i++)
                    {
                        Renderable renderable = renderables[i].renderable;
                        uint32_t count = renderables[i].count;

                        VKMaterialHandle material = renderable.material.DynamicCastHandle<VKMaterial>();
                        VKMeshHandle     mesh = renderable.mesh.DynamicCastHandle<VKMesh>();
                    
                        std::vector<VkDescriptorSet> descriptorSets = material->GetVKDescriptorSets();
                        
                        //Bind material descriptor sets
                        vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vkPipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
                    
                        //Bind instance buffer
                        if(m_instanceDataSize > 0)
                            vkCmdBindVertexBuffers(m_commandBuffer, 1, 1, &m_instanceBlock.buffer, offsets);

                        UniformBlock_vk vertBlock = mesh->GetVertexBlock();
                        UniformBlock_vk indexBlock = mesh->GetIndexBlock();
                        uint32_t indexCount = mesh->GetIndexCount();

                        vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &vertBlock.buffer, offsets);
                        vkCmdBindIndexBuffer(m_commandBuffer, indexBlock.buffer, 0, VK_INDEX_TYPE_UINT32);
                        
                        vkCmdDrawIndexed(m_commandBuffer, indexCount, count, 0, 0, 0);
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

                //Delete instance data
                delete[] m_instanceData;
                m_instanceData = nullptr;
                m_instanceDataSize = 0;

                return true;
            }

            const VkRenderPass& VKRenderPass::GetVkRenderPass() const { return m_renderPass; }

            const VkCommandBuffer& VKRenderPass::GetVkCommandBuffer() const { return m_commandBuffer; }

            const std::vector<IRenderTargetHandle>& VKRenderPass::GetOutputRenderTargets() const { return m_outputRenderTargets; }

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
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthAttachment.flags = 0;

                attachmentDescriptions.push_back(depthAttachment);

                VkAttachmentReference depthReference = {};
                depthReference.attachment = static_cast<uint32_t>(attachmentReferences.size());
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
                    Image_vk colorImage;

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
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

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

            bool VKRenderPass::setupDescriptorSets(std::map<uint32_t, std::map<uint32_t, VKRenderTargetHandle>> inputTargets)
            {
                if (inputTargets.size() <= 0)
                    return true;

                VkResult err;

                //Get the root layout so that we can determine which set layouts we'll need
                std::vector<VkDescriptorSetLayout> allDescriptorSetLayouts = VKRenderer::RendererInstance->GetVKRootLayoutHandle()->VKGetDescriptorSetLayouts();

                //Collect every descriptor set layout that will show up
                bool recordedFirstSetIndex = false;

                std::vector<VkDescriptorSetLayout> usedDescriptorSetLayouts;
                for (auto it = inputTargets.begin(); it != inputTargets.end(); it++)
                {
                    uint32_t setIndex = it->first;
                    usedDescriptorSetLayouts.push_back(allDescriptorSetLayouts[setIndex]);

                    if (!recordedFirstSetIndex)
                    {
                        m_firstInputTargetSetIndex = setIndex;
                        recordedFirstSetIndex = true;
                    }
                }

                //Allocate space for every descriptor set at once; each entry in the top level map (inputTargets) is a set
                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = m_descriptorPool;
                allocInfo.descriptorSetCount = static_cast<uint32_t>(inputTargets.size());
                allocInfo.pSetLayouts = usedDescriptorSetLayouts.data();

                m_inputTargetDescriptorSets.resize(inputTargets.size());
                err = vkAllocateDescriptorSets(m_device, &allocInfo, m_inputTargetDescriptorSets.data());
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKRenderPass::setupDescriptorSets(): Failed to allocate descriptor set\n");
                    return false;
                }

                //Setup descriptor set writes
                std::vector<VkWriteDescriptorSet> descSetWrites;
                //Store texture descriptors first so that they can stay on the stack long enough
                std::vector<VkDescriptorImageInfo> targetDescriptors;

                uint32_t index = 0;
                for (auto it = inputTargets.begin(); it != inputTargets.end(); it++)
                {
                    uint32_t setIndex = it->first;

                    std::map<uint32_t, VKRenderTargetHandle> targetBindings = it->second;

                    
                    for (auto it = targetBindings.begin(); it != targetBindings.end(); it++)
                    {
                        VKRenderTargetHandle targetHandle = it->second;
                        Texture_vk texture = targetHandle->GetVKTexture();

                        //Create Texture description
                        VkDescriptorImageInfo targetDescriptor = {};
                        targetDescriptor.sampler = texture.sampler;
                        targetDescriptor.imageView = texture.image.view;
                        targetDescriptor.imageLayout = texture.layout;

                        targetDescriptors.push_back(targetDescriptor);
                    }

                    uint32_t targetIndex = 0;
                    for (auto it = targetBindings.begin(); it != targetBindings.end(); it++)
                    {
                        VkWriteDescriptorSet inputTextureWrite = {};
                        inputTextureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        inputTextureWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        inputTextureWrite.dstSet = m_inputTargetDescriptorSets[index];
                        inputTextureWrite.dstBinding = it->first;
                        inputTextureWrite.pImageInfo = &targetDescriptors[targetIndex];
                        inputTextureWrite.descriptorCount = 1;

                        descSetWrites.push_back(inputTextureWrite);
                        targetIndex++;
                    }
                    index++;
                }

                vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descSetWrites.size()), descSetWrites.data(), 0, nullptr);

                return true;
            }
        }
    }
}
