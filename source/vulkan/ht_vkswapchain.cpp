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


#include <ht_vkswapchain.h>
#include <ht_vkrenderer.h>
#include <ht_vkrenderer.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKSwapchain::VKSwapchain(VkInstance* instance, VkDevice* device, VkCommandPool* commandPool)
            {
                m_swapchain = 0;

                m_instance = instance;
                m_device = device;
                m_commandPool = commandPool;
            }

            VKSwapchain::~VKSwapchain()
            {
                VkDevice device = *m_device;
                VkCommandPool commandPool = *m_commandPool;

                vkDestroyImageView(device, m_depthBuffer.view, nullptr);
                vkDestroyImage(device, m_depthBuffer.image, nullptr);
                vkFreeMemory(device, m_depthBuffer.memory, nullptr);

                uint32_t i;

                for (i = 0; i < m_swapchainBuffers.size(); i++)
                    vkDestroyFramebuffer(device, m_framebuffers[i], nullptr);
                m_framebuffers.clear();

                for (i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    vkDestroyImageView(device, m_swapchainBuffers[i].view, nullptr);
                    vkFreeCommandBuffers(device, commandPool, 1, &m_swapchainBuffers[i].command);
                }
                vkDestroyCommandPool(device, commandPool, nullptr);
                m_swapchainBuffers.clear();
            }

            VkCommandBuffer VKSwapchain::GetCurrentCommand()
            {
                return m_swapchainBuffers[m_currentBuffer].command;
            }

            bool VKSwapchain::VKPrepare(VkSurfaceKHR surface, VkColorSpaceKHR colorSpace, VkPresentModeKHR* presentModes, uint32_t presentModeCount, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D extents)
            {
                //Setup function pointers that we'll need
                setupFunctionPointers();

                m_width = extents.width;
                m_height = extents.height;

                VKRenderer* renderer = VKRenderer::RendererInstance;
                VkFormat preferredColorFormat = renderer->GetPreferredImageFormat();
                /*
                    Prepare Color
                */
                if (!prepareSwapchain(renderer, preferredColorFormat, surface, colorSpace, presentModes, presentModeCount, surfaceCapabilities, extents))
                    return false;

                /*
                    Prepare depth
                */
                if (!prepareSwapchainDepth(renderer, extents))
                    return false;
                
                /*
                    Prepare internal render pass
                */
                if (!prepareRenderPass(preferredColorFormat))
                    return false;

                /*
                    Prepare framebuffers
                */
                if (!prepareFramebuffers(extents))
                    return false;

                /*
                    Allocate space for the swapchain command buffers
                */
                if (!allocateCommandBuffers())
                    return false;

                return true;
            }

            bool VKSwapchain::BuildSwapchain(VkClearValue clearColor)
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
                clearValues[0] = clearColor;
                clearValues[1] = { 1.0f, 0 };

                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++) 
                {
                    VkRenderPassBeginInfo renderPassBeginInfo = {};
                    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                    renderPassBeginInfo.pNext = nullptr;
                    renderPassBeginInfo.renderPass = m_renderPass;
                    renderPassBeginInfo.framebuffer = m_framebuffers[i];
                    renderPassBeginInfo.renderArea.offset.x = 0;
                    renderPassBeginInfo.renderArea.offset.y = 0;
                    renderPassBeginInfo.renderArea.extent.width = m_width;
                    renderPassBeginInfo.renderArea.extent.height = m_height;
                    renderPassBeginInfo.clearValueCount = 2;
                    renderPassBeginInfo.pClearValues = clearValues;

                    VkCommandBuffer commandBuffer = m_swapchainBuffers[i].command;

                    err = vkBeginCommandBuffer(commandBuffer, &beginInfo);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::buildCommandBuffer(): Failed to build command buffer.\n");
#endif
                        return false;
                    }

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

                    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
                    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

                    //TODO: Draw fullscreen Tri

                    vkCmdEndRenderPass(commandBuffer);

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

                    prePresentBarrier.image = m_swapchainBuffers[i].image;

                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

                    err = vkEndCommandBuffer(commandBuffer);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::buildCommandBuffer(): Failed to end command buffer.\n");
#endif
                        return false;
                    }
                }

                return true;
            }

            VkResult VKSwapchain::VKGetNextImage(VkSemaphore presentSemaphore)
            {
                //TODO: Use fences
                VkDevice device = VKRenderer::RendererInstance->GetVKDevice();
                return fpAcquireNextImageKHR(device, m_swapchain, UINT64_MAX, presentSemaphore, VK_NULL_HANDLE, &m_currentBuffer);
            }

            VkResult VKSwapchain::VKPresent(VkQueue queue)
            {
                VkPresentInfoKHR present = {};
                present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                present.pNext = nullptr;
                present.swapchainCount = 1;
                present.pSwapchains = &m_swapchain;
                present.pImageIndices = &m_currentBuffer;

                return fpQueuePresentKHR(queue, &present);
            }

            /*
                Private Methods
            */

            void VKSwapchain::setupFunctionPointers()
            {
                //Pointer to function to get function pointers from device
                PFN_vkGetDeviceProcAddr g_gdpa = (PFN_vkGetDeviceProcAddr)
                    vkGetInstanceProcAddr(*m_instance, "vkGetDeviceProcAddr");

                VkDevice device = *m_device;

                fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(device, "vkCreateSwapchainKHR");
                fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(device, "vkDestroySwapchainKHR");
                fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(device, "vkGetSwapchainImagesKHR");
                fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(device, "vkAcquireNextImageKHR");
                fpQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(device, "vkQueuePresentKHR");
            }

            bool VKSwapchain::prepareSwapchain(VKRenderer* renderer, VkFormat preferredColorFormat, VkSurfaceKHR surface, VkColorSpaceKHR colorSpace, VkPresentModeKHR* presentModes, uint32_t presentModeCount, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D surfaceExtents)
            {
                VkResult err;
                VkSwapchainKHR oldSwapchain = m_swapchain;

                VkDevice device = *m_device;

                //Use mailbox mode if available as it's the lowest-latency non-tearing mode
                //If that's not available try immediate mode which SHOULD be available and is fast but tears
                //Fall back to FIFO which is always available
                VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
                for (size_t i = 0; i < presentModeCount; i++) {
                    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                    }
                    if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
                        (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
                        swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                    }
                }

                //Determine how many VkImages to use in the swap chain
                //We only own one at a time besides the image being displayed
                uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount + 1;
                if ((surfaceCapabilities.maxImageCount > 0) &
                    (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
                    desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;

                //Setup transform flags
                VkSurfaceTransformFlagBitsKHR preTransform;
                if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                else
                    preTransform = surfaceCapabilities.currentTransform;

                //Create swapchain

                VkSwapchainCreateInfoKHR swapchainInfo;
                swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                swapchainInfo.pNext = nullptr;
                swapchainInfo.surface = surface;
                swapchainInfo.minImageCount = desiredNumberOfSwapchainImages;
                swapchainInfo.imageFormat = preferredColorFormat;
                swapchainInfo.imageColorSpace = colorSpace;
                swapchainInfo.imageExtent = surfaceExtents;
                swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                swapchainInfo.preTransform = preTransform;
                swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                swapchainInfo.imageArrayLayers = 1;
                swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swapchainInfo.queueFamilyIndexCount = 0;
                swapchainInfo.pQueueFamilyIndices = nullptr;
                swapchainInfo.presentMode = swapchainPresentMode;
                swapchainInfo.oldSwapchain = oldSwapchain;
                swapchainInfo.clipped = true;
                swapchainInfo.flags = 0;

                uint32_t i; // About to be used for a bunch of loops

                err = fpCreateSwapchainKHR(device, &swapchainInfo, nullptr, &m_swapchain);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to create Swapchain.\n");
#endif
                    return false;
                }

                //Destroy old swapchain
                if (oldSwapchain != VK_NULL_HANDLE)
                    fpDestroySwapchainKHR(device, oldSwapchain, nullptr);

                //Get the swapchain images
                uint32_t swapchainImageCount;
                err = fpGetSwapchainImagesKHR(device, m_swapchain, &swapchainImageCount, nullptr);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to get swapchain image count.\n");
#endif
                    return false;
                }

                std::vector<VkImage> swapchainImages;
                swapchainImages.resize(swapchainImageCount);
                err = fpGetSwapchainImagesKHR(device, m_swapchain, &swapchainImageCount, &swapchainImages[0]);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to get swapchain images.\n");
#endif
                    return false;
                }

                for (i = 0; i < swapchainImageCount; i++)
                {
                    VkComponentMapping components;
                    components.r = VK_COMPONENT_SWIZZLE_R;
                    components.g = VK_COMPONENT_SWIZZLE_G;
                    components.b = VK_COMPONENT_SWIZZLE_B;
                    components.a = VK_COMPONENT_SWIZZLE_A;

                    VkImageSubresourceRange subresourceRange;
                    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    subresourceRange.baseMipLevel = 0;
                    subresourceRange.levelCount = 1;
                    subresourceRange.baseArrayLayer = 0;
                    subresourceRange.layerCount = 1;

                    VkImageViewCreateInfo colorImageView;
                    colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    colorImageView.pNext = nullptr;
                    colorImageView.format = preferredColorFormat;
                    colorImageView.components = components;
                    colorImageView.subresourceRange = subresourceRange;
                    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    colorImageView.flags = 0;

                    SwapchainBuffer buffer;
                    buffer.image = swapchainImages[i];

                    //Render loop will expect image to have been used before
                    //Init image ot the VK_IMAGE_ASPECT_COLOR_BIT state
                    VkCommandBuffer setupCommand = renderer->GetSetupCommandBuffer();
                    renderer->SetImageLayout(setupCommand, buffer.image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

                    colorImageView.image = buffer.image;

                    //Attempt to create the image view
                    err = vkCreateImageView(device, &colorImageView, nullptr, &buffer.view);

                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to create image view.\n");
#endif
                        return false;
                    }

                    m_swapchainBuffers.push_back(buffer);
                }

                return true;
            }

            bool VKSwapchain::prepareSwapchainDepth(VKRenderer* renderer, VkExtent2D extent)
            {
                VkResult err;
                const VkFormat depthFormat = renderer->GetPreferredDepthFormat();

                VkDevice device = *m_device;

                VkImageCreateInfo image;
                image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                image.pNext = nullptr;
                image.imageType = VK_IMAGE_TYPE_2D;
                image.format = depthFormat;
                image.extent = { extent.width, extent.height, 1 };
                image.mipLevels = 1;
                image.arrayLayers = 1;
                image.samples = VK_SAMPLE_COUNT_1_BIT;
                image.tiling = VK_IMAGE_TILING_OPTIMAL;
                image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                image.flags = 0;
                image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                image.queueFamilyIndexCount = 0;
                image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                VkComponentMapping components;
                components.r = VK_COMPONENT_SWIZZLE_R;
                components.g = VK_COMPONENT_SWIZZLE_G;
                components.b = VK_COMPONENT_SWIZZLE_B;
                components.a = VK_COMPONENT_SWIZZLE_A;

                VkImageSubresourceRange depthSubresourceRange;
                depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                depthSubresourceRange.baseMipLevel = 0;
                depthSubresourceRange.levelCount = 1;
                depthSubresourceRange.baseArrayLayer = 0;
                depthSubresourceRange.layerCount = 1;

                VkImageViewCreateInfo view;
                view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view.pNext = nullptr;
                view.image = VK_NULL_HANDLE;
                view.format = depthFormat;
                view.subresourceRange = depthSubresourceRange;
                view.flags = 0;
                view.components = components;
                view.viewType = VK_IMAGE_VIEW_TYPE_2D;

                m_depthBuffer.format = depthFormat;

                VkMemoryRequirements memoryRequirements;
                bool pass;

                //Create image
                err = vkCreateImage(device, &image, nullptr, &m_depthBuffer.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKSwapchain::prepareSwapchainDepth(): Error, failed to create image\n");
#endif
                    return false;
                }

                vkGetImageMemoryRequirements(device, m_depthBuffer.image, &memoryRequirements);

                m_depthBuffer.memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                m_depthBuffer.memAllocInfo.pNext = nullptr;
                m_depthBuffer.memAllocInfo.allocationSize = memoryRequirements.size;
                m_depthBuffer.memAllocInfo.memoryTypeIndex = 0;

                //No requirements
                pass = renderer->MemoryTypeFromProperties(memoryRequirements.memoryTypeBits, 0, &m_depthBuffer.memAllocInfo.memoryTypeIndex);
                assert(pass);
                if (!pass)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKSwapchain::prepareSwapchainDepth(): Error, failed to get memory type for depth buffer\n");
#endif
                    return false;
                }

                //Allocate Memory
                err = vkAllocateMemory(device, &m_depthBuffer.memAllocInfo, nullptr, &m_depthBuffer.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKSwapchain::prepareSwapchainDepth(): Error, failed to allocate memory for depth buffer\n");
#endif
                    return false;
                }

                //Bind Memory
                err = vkBindImageMemory(device, m_depthBuffer.image, m_depthBuffer.memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKSwapchain::prepareSwapchainDepth(): Error, failed to bind depth buffer memory\n");
#endif
                    return false;
                }

                VkCommandBuffer setupCommand = renderer->GetSetupCommandBuffer();
                renderer->SetImageLayout(setupCommand, m_depthBuffer.image, VK_IMAGE_ASPECT_DEPTH_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

                //Create image view
                view.image = m_depthBuffer.image;
                err = vkCreateImageView(device, &view, nullptr, &m_depthBuffer.view);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKSwapchain::prepareSwapchainDepth(): Error, failed create image view for depth buffer\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKSwapchain::prepareRenderPass(VkFormat preferredColorFormat) 
            {
                VkAttachmentDescription attachments[2];
                attachments[0].format = preferredColorFormat;
                attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments[0].flags = 0;

                attachments[1].format = m_depthBuffer.format;
                attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
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

                err = vkCreateRenderPass(*m_device, &renderPassInfo, nullptr, &m_renderPass);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKSwapchain::prepareRenderPass(): Failed to create render pass\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKSwapchain::prepareFramebuffers(VkExtent2D extents)
            {
                VkResult err;

                VkImageView attachments[2] = {};
                attachments[1] = m_depthBuffer.view;

                VkFramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.renderPass = m_renderPass;
                framebufferInfo.attachmentCount = 2;
                framebufferInfo.pAttachments = attachments;
                framebufferInfo.width = extents.width;
                framebufferInfo.height = extents.height;
                framebufferInfo.layers = 1;

                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    attachments[0] = m_swapchainBuffers[i].view;
                    VkFramebuffer framebuffer;
                    err = vkCreateFramebuffer(*m_device, &framebufferInfo, nullptr, &framebuffer);

                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKSwapchain::prepareFrambuffers(): Failed to create framebuffer at index:%d \n", i);
#endif
                        return false;
                    }

                    m_framebuffers.push_back(framebuffer);
                }

                return true;
            }

            bool VKSwapchain::allocateCommandBuffers() 
            {
                VkResult err;

                VkCommandBufferAllocateInfo allocateInfo;
                allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocateInfo.pNext = nullptr;
                allocateInfo.commandPool = *m_commandPool;
                allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocateInfo.commandBufferCount = 1;

                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    err = vkAllocateCommandBuffers(*m_device, &allocateInfo, &m_swapchainBuffers[i].command);
                    assert(!err);

                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKSwapchain::allocateCommandBuffers(): Failed to allocate for command buffer at index:%d \n", i);
#endif
                        return false;
                    }
                }

                return true;
            }
        }
    }
}