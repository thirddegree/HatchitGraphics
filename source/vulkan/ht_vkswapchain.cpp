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

#include <ht_VKSwapChain.h>
#include <ht_vkrenderer.h>
#include <ht_vkrootlayout.h>
#include <ht_rootlayout.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Resource;

            VKSwapChain::VKSwapChain(const RendererParams& rendererParams, VKDevice* device)
            {
                m_swapchain = VK_NULL_HANDLE;

                m_device = device->GetVKDevices()[0];
                m_instance = device->GetVKInstance();
                m_gpu = device->GetVKPhysicalDevices()[0];

                if (!prepareSurface(rendererParams))
                    HT_DEBUG_PRINTF("VKSwapChain(): Failed to prepare surface");
            }

            VKSwapChain::~VKSwapChain()
            {
                destroyPipeline();

                destroyDepth();

                destroySwapchainBuffers();

                destroyFramebuffers();

                destroyRenderPass();

                destroySwapchain();

                destroySurface();
            }

            const VkCommandBuffer& VKSwapChain::VKGetCurrentCommand()
            {
                return m_swapchainBuffers[m_currentBuffer].command;
            }

            const VkSurfaceKHR& VKSwapChain::VKGetSurface()
            {
                return m_surface;
            }
            const uint32_t& VKSwapChain::VKGetGraphicsQueueIndex()
            {
                return m_graphicsQueueNodeIndex;
            }
            const VkFormat& VKSwapChain::VKGetPreferredColorFormat()
            {
                return m_preferredColorFormat;
            }
            const VkFormat& VKSwapChain::VKGetPreferredDepthFormat()
            {
                return m_preferredDepthFormat;
            }

            bool VKSwapChain::VKPrepare()
            {
                VkResult err;

                bool recreate = false;
                //Clean out old data
                if (m_swapchain != VK_NULL_HANDLE)
                {
                    destroyDepth();

                    destroySwapchainBuffers();

                    destroyFramebuffers();

                    m_swapchainBuffers.clear();

                    recreate = true;
                }

                // Get physical device surface properties and formats
                VkSurfaceCapabilitiesKHR surfCaps;
                err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, m_surface, &surfCaps);
                assert(!err);

                // Get available present modes
                uint32_t presentModeCount;
                err = fpGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &presentModeCount, NULL);
                assert(!err);
                assert(presentModeCount > 0);

                std::vector<VkPresentModeKHR> presentModes(presentModeCount);

                err = fpGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &presentModeCount, presentModes.data());
                assert(!err);

                VkExtent2D swapchainExtent = {};
                // width and height are either both -1, or both not -1.
                if (surfCaps.currentExtent.width == -1)
                {
                    // If the surface size is undefined, the size is set to
                    // the size of the images requested.
                    swapchainExtent.width = m_width;
                    swapchainExtent.height = m_height;
                }
                else
                {
                    // If the surface size is defined, the swap chain size must match
                    swapchainExtent = surfCaps.currentExtent;
                    m_width = surfCaps.currentExtent.width;
                    m_height = surfCaps.currentExtent.height;
                }

                // Prefer mailbox mode if present, it's the lowest latency non-tearing present  mode
                VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
                for (size_t i = 0; i < presentModeCount; i++)
                {
                    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                    {
                        swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                    }
                    if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
                    {
                        swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                    }
                }

                // Determine the number of images
                uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
                if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
                {
                    desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
                }

                VkSurfaceTransformFlagsKHR preTransform;
                if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                {
                    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                }
                else
                {
                    preTransform = surfCaps.currentTransform;
                }

                /*
                    Prepare Color
                */
                if (!prepareSwapchain(m_preferredColorFormat, m_colorSpace,
                    presentModes, surfCaps, swapchainExtent))
                    return false;

                /*
                    Prepare depth
                */
                if (!prepareSwapchainDepth(m_preferredDepthFormat, swapchainExtent))
                    return false;

                /*
                    Prepare internal render pass
                */
                if (!recreate && !prepareRenderPass())
                    return false;

                /*
                    Prepare framebuffers
                */
                if (!prepareFramebuffers(swapchainExtent))
                    return false;

                return true;
            }

            bool VKSwapChain::VKPrepareResources()
            {
                VkResult err;

                Resource::Pipeline::RasterizerState rasterState = {};
                rasterState.cullMode = Resource::Pipeline::CullMode::NONE;
                rasterState.polygonMode = Resource::Pipeline::PolygonMode::SOLID;
                rasterState.depthClampEnable = true;

                Resource::Pipeline::MultisampleState multisampleState = {};
                multisampleState.minSamples = 0;
                multisampleState.samples = Resource::Pipeline::SAMPLE_1_BIT;

                PipelineHandle pipelineHandle = Pipeline::GetHandle("SwapchainPipeline.json", "SwapchainPipeline.json");
                m_pipeline = static_cast<VKPipeline*>(pipelineHandle->GetBase());

                //Setup the descriptor sets
                Graphics::RootLayoutHandle rootLayoutHandle = Graphics::RootLayout::GetHandle("TestRootDescriptor.json", "TestRootDescriptor.json");
                VKRootLayout* rootLayout = static_cast<VKRootLayout*>(rootLayoutHandle->GetBase());
                const std::vector<VkDescriptorSetLayout> descriptorSetLayouts = rootLayout->VKGetDescriptorSetLayouts();

                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = m_descriptorPool;
                allocInfo.pSetLayouts = &descriptorSetLayouts[3];
                allocInfo.descriptorSetCount = 1;

                err = vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareResources: Failed to allocate descriptor set\n");
                    return false;
                }

                std::vector<VkDescriptorImageInfo> textureDescriptors;
                std::vector<VkWriteDescriptorSet> descriptorWrites;

                for (size_t i = 0; i < m_inputTextures.size(); i++)
                {
                    Texture_vk inputTexture = m_inputTextures[i];

                    // Image descriptor for the color map texture
                    VkDescriptorImageInfo texDescriptor = {};
                    texDescriptor.sampler = inputTexture.sampler;
                    texDescriptor.imageView = inputTexture.image.view;
                    texDescriptor.imageLayout = inputTexture.layout;

                    textureDescriptors.push_back(texDescriptor);
                }

                for (size_t i = 0; i < m_inputTextures.size(); i++)
                {
                    VkWriteDescriptorSet uniformTexure2DWrite = {};
                    uniformTexure2DWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    uniformTexure2DWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    uniformTexure2DWrite.dstSet = m_descriptorSet;
                    uniformTexure2DWrite.dstBinding = static_cast<uint32_t>(i);
                    uniformTexure2DWrite.pImageInfo = &textureDescriptors[i];
                    uniformTexure2DWrite.descriptorCount = 1;

                    descriptorWrites.push_back(uniformTexure2DWrite);
                }

                vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

                //Buffer 3 blank points
                float blank[9] = { 0,0,0,0,0,0,0,0,0 };
                if (!CreateUniformBuffer(m_device, 9, blank, &m_vertexBuffer))
                    return false;

                m_vertexBuffer.descriptor.offset = 0;
                m_vertexBuffer.descriptor.range = 9;

                return true;
            }

            bool VKSwapChain::BuildSwapchainCommands(VkClearValue clearColor)
            {
                /*
                    Allocate space for the swapchain command buffers
                */
                if (!allocateCommandBuffers())
                    return false;

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

                Graphics::RootLayoutHandle rootLayoutHandle = Graphics::RootLayout::GetHandle("TestRootDescriptor.json", "TestRootDescriptor.json");
                VKRootLayout* rootLayout = static_cast<VKRootLayout*>(rootLayoutHandle->GetBase());
                VkPipelineLayout pipelineLayout = rootLayout->VKGetPipelineLayout();

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
                        HT_DEBUG_PRINTF("VKSwapChain::BuildSwapchain(): Failed to build command buffer.\n");
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

                    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                        3, 1, &m_descriptorSet, 0, nullptr);
                    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetVKPipeline());

                    //Draw fullscreen Tri; geometry created in shader
                    VkDeviceSize offsets = { 0 };
                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_vertexBuffer.buffer, &offsets);
                    vkCmdBindVertexBuffers(commandBuffer, 1, 1, &m_vertexBuffer.buffer, &offsets);
                    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

                    vkCmdEndRenderPass(commandBuffer);

                    err = vkEndCommandBuffer(commandBuffer);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKSwapChain::BuildSwapchain(): Failed to end command buffer.\n");
                        return false;
                    }

                    //Setup barrier commands


                    VkCommandBufferBeginInfo cmdBufInfo = {};
                    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    cmdBufInfo.pInheritanceInfo = nullptr;

                    err = vkBeginCommandBuffer(m_postPresentCommands[i], &cmdBufInfo);
                    assert(!err);

                    VkImageMemoryBarrier postPresentBarrier = {};
                    postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    postPresentBarrier.srcAccessMask = 0;
                    postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    postPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                    postPresentBarrier.image = m_swapchainBuffers[i].image;

                    vkCmdPipelineBarrier(
                        m_postPresentCommands[i],
                        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        0,
                        0, nullptr, // No memory barriers,
                        0, nullptr, // No buffer barriers,
                        1, &postPresentBarrier);

                    err = vkEndCommandBuffer(m_postPresentCommands[i]);
                    assert(!err);


                    err = vkBeginCommandBuffer(m_prePresentCommands[i], &cmdBufInfo);
                    assert(!err);

                    VkImageMemoryBarrier prePresentBarrier = {};
                    prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    prePresentBarrier.dstAccessMask = 0;
                    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                    prePresentBarrier.image = m_swapchainBuffers[i].image;

                    vkCmdPipelineBarrier(
                        m_prePresentCommands[i],
                        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        0,
                        0, nullptr, // No memory barriers,
                        0, nullptr, // No buffer barriers,
                        1, &prePresentBarrier);

                    err = vkEndCommandBuffer(m_prePresentCommands[i]);
                    assert(!err);

                }

                return true;
            }

            VkResult VKSwapChain::VKGetNextImage(VkSemaphore presentSemaphore)
            {
                //TODO: Use fences
                return fpAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, presentSemaphore, VK_NULL_HANDLE, &m_currentBuffer);
            }

            bool VKSwapChain::VKPostPresentBarrier(const VkQueue& queue)
            {
                return submitBarrier(queue, m_postPresentCommands[m_currentBuffer]);
            }
            bool VKSwapChain::VKPrePresentBarrier(const VkQueue& queue)
            {
                return submitBarrier(queue, m_prePresentCommands[m_currentBuffer]);
            }

            VkResult VKSwapChain::VKPresent(const VkQueue& queue, const VkSemaphore& renderSemaphore)
            {
                VkPresentInfoKHR present = {};
                present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                present.pNext = nullptr;
                present.swapchainCount = 1;
                present.pSwapchains = &m_swapchain;
                present.pImageIndices = &m_currentBuffer;

                if (renderSemaphore != VK_NULL_HANDLE)
                {
                    present.pWaitSemaphores = &renderSemaphore;
                    present.waitSemaphoreCount = 1;
                }

                return fpQueuePresentKHR(queue, &present);
            }

            void VKSwapChain::VKSetIncomingRenderPass(VKRenderPassHandle renderPass)
            {
                std::vector<RenderTargetHandle> incomingRenderTargets = renderPass->GetOutputRenderTargets();
                for (size_t i = 0; i < incomingRenderTargets.size(); i++)
                {
                    VKRenderTarget* vkRenderTarget = static_cast<VKRenderTarget*>(incomingRenderTargets[i]->GetBase());
                    m_inputTextures.push_back(vkRenderTarget->GetVKTexture());
                }
            }

            /*
                Private Methods
            */

            bool VKSwapChain::createAllocatorPools()
            {
                VkResult err;

                VkCommandPoolCreateInfo commandPoolInfo = {};
                commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                commandPoolInfo.pNext = nullptr;
                commandPoolInfo.flags = 0;
                commandPoolInfo.queueFamilyIndex = m_graphicsQueueNodeIndex;
               
                err = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool);
                assert(!err);

                std::vector<VkDescriptorPoolSize> poolSizes;

                VkDescriptorPoolSize imageSize = {};
                imageSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                imageSize.descriptorCount = 2;

                poolSizes.push_back(imageSize);

                VkDescriptorPoolCreateInfo poolCreateInfo = {};
                poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolCreateInfo.pPoolSizes = poolSizes.data();
                poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
                poolCreateInfo.maxSets = 2;
                poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

                err = vkCreateDescriptorPool(m_device, &poolCreateInfo, nullptr, &m_descriptorPool);
                assert(!err);

                return true;
            }

            bool VKSwapChain::prepareSurface(const RendererParams& rendererParams) 
            {
                VkResult err;

                //Hook into the window
#ifdef HT_SYS_WINDOWS
                //Get HINSTANCE from HWND
                HWND window = (HWND)rendererParams.window;
                HINSTANCE instance;
                instance = (HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE);

                VkWin32SurfaceCreateInfoKHR creationInfo;
                creationInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0; //Unused in Vulkan 1.0.3;
                creationInfo.hinstance = instance;
                creationInfo.hwnd = window;

                err = vkCreateWin32SurfaceKHR(m_instance, &creationInfo, nullptr, &m_surface);

                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("Error creating VkSurface for Win32 window");
                    return false;
                }
#endif

#ifdef HT_SYS_LINUX
                VkXcbSurfaceCreateInfoKHR creationInfo;
                creationInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0;
                creationInfo.connection = (xcb_connection_t*)params.display;
                creationInfo.window = *(uint32_t*)params.window;

                err = vkCreateXcbSurfaceKHR(m_instance, &creationInfo, nullptr, &m_surface);

                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSurface(): Error creating VkSurface for Xcb window");

                    return false;
                }
#endif
                if(!getQueueProperties())
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSurface(): Error getting queue properties");

                if(!findSutibleQueue())
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSurface(): Could not find suitible queue");

                if (!getPreferredFormats())
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSurface(): Error getting preffered image formats");

                if (!createAllocatorPools())
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSurface(): Error creating allocator pools");

                return true;
            }

            bool VKSwapChain::getQueueProperties() 
            {
                vkGetPhysicalDeviceProperties(m_gpu, &m_gpuProps);

                //Call with NULL data to get count
                uint32_t queueCount;
                vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueCount, NULL);
                assert(queueCount >= 1);

                if (queueCount == 0)
                {
                    HT_DEBUG_PRINTF("VKRenderer::setupDeviceQueues: No queues were found on the device\n");
                    return false;
                }

                m_queueProps = std::vector<VkQueueFamilyProperties>(queueCount);
                vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueCount, &m_queueProps[0]);

                // Find a queue that supports gfx
                uint32_t gfxQueueIdx = 0;
                for (gfxQueueIdx = 0; gfxQueueIdx < queueCount; gfxQueueIdx++) {
                    if (m_queueProps[gfxQueueIdx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        break;
                }
                assert(gfxQueueIdx < queueCount);

                if (gfxQueueIdx >= queueCount)
                {
                    HT_DEBUG_PRINTF("VKRenderer::setupDeviceQueues: No graphics queue was found on the device\n");
                    return false;
                }

                return true;
            }

            bool VKSwapChain::findSutibleQueue() 
            {
                uint32_t i; //we reuse this for all the loops

                            //Find which queue we can use to present
                VkBool32* supportsPresent = new VkBool32[m_queueProps.size()];
                for (i = 0; i < m_queueProps.size(); i++)
                    fpGetPhysicalDeviceSurfaceSupportKHR(m_gpu, i, m_surface, &supportsPresent[i]);

                //Search for a queue that can both do graphics and presentation
                uint32_t graphicsQueueNodeIndex = UINT32_MAX;
                uint32_t presentQueueNodeIndex = UINT32_MAX;

                for (i = 0; i < m_queueProps.size(); i++) {
                    if ((m_queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                        if (graphicsQueueNodeIndex == UINT32_MAX)
                            graphicsQueueNodeIndex = i;

                        if (supportsPresent[i] == VK_TRUE) {
                            graphicsQueueNodeIndex = i;
                            presentQueueNodeIndex = i;
                            break;
                        }
                    }
                }
                if (presentQueueNodeIndex == UINT32_MAX) {
                    // If didn't find a queue that supports both graphics and present, then
                    // find a separate present queue.
                    for (uint32_t i = 0; i < m_queueProps.size(); ++i) {
                        if (supportsPresent[i] == VK_TRUE) {
                            presentQueueNodeIndex = i;
                            break;
                        }
                    }
                }

                delete[] supportsPresent;

                // Generate error if could not find both a graphics and a present queue
                if (graphicsQueueNodeIndex == UINT32_MAX ||
                    presentQueueNodeIndex == UINT32_MAX) {
                    HT_DEBUG_PRINTF("Unable to find a graphics and a present queue.\n");
                    return false;
                }

                //Save the index of the queue we want to use
                m_graphicsQueueNodeIndex = graphicsQueueNodeIndex;

                return true;
            }

            bool VKSwapChain::getPreferredFormats() 
            {
                VkResult err;

                //Get list of supported VkFormats
                uint32_t formatCount;
                err = fpGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &formatCount, nullptr);

                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VkRenderer::getSupportedFormats(): Error getting number of formats from device.\n");
                    return false;
                }

                //Get format list
                VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount];
                err = fpGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &formatCount, surfaceFormats);
                if (err != VK_SUCCESS || formatCount <= 0)
                {
                    HT_DEBUG_PRINTF("VkRenderer::getSupportedFormats(): Error getting VkSurfaceFormats from device.\n");
                    return false;
                }

                // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
                // the surface has no preferred format.  Otherwise, at least one
                // supported format will be returned.
                if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
                    m_preferredColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
                else
                    m_preferredColorFormat = surfaceFormats[0].format;

                //Try to find the preferred depth format

                // Since all depth formats may be optional, we need to find a suitable depth format to use
                // Start with the highest precision packed format
                std::vector<VkFormat> depthFormats = {
                    VK_FORMAT_D32_SFLOAT_S8_UINT,
                    VK_FORMAT_D32_SFLOAT,
                    VK_FORMAT_D24_UNORM_S8_UINT,
                    VK_FORMAT_D16_UNORM_S8_UINT,
                    VK_FORMAT_D16_UNORM
                };

                //Default to VK_FORMAT_D32_SFLOAT just because
                m_preferredDepthFormat = VK_FORMAT_D32_SFLOAT;

                for (VkFormat format : depthFormats)
                {
                    VkFormatProperties formatProps;
                    vkGetPhysicalDeviceFormatProperties(m_gpu, format, &formatProps);
                    // Format must support depth stencil attachment for optimal tiling
                    if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                    {
                        m_preferredDepthFormat = format;
                        break;
                    }
                }

                //Get the color space we're expected to work in
                m_colorSpace = surfaceFormats[0].colorSpace;

                return true;
            }

            bool VKSwapChain::prepareSwapchain(VkFormat preferredColorFormat, VkColorSpaceKHR colorSpace, std::vector<VkPresentModeKHR> presentModes, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkExtent2D surfaceExtents)
            {
                VkResult err;
                VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
                if (m_swapchain != VK_NULL_HANDLE)
                    oldSwapchain = m_swapchain;

                //Use mailbox mode if available as it's the lowest-latency non-tearing mode
                //If that's not available try immediate mode which SHOULD be available and is fast but tears
                //Fall back to FIFO which is always available
                VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
                for (size_t i = 0; i < presentModes.size(); i++) {
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
                swapchainInfo.flags = 0;
                swapchainInfo.pNext = nullptr;
                swapchainInfo.surface = m_surface;
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

                uint32_t i; // About to be used for a bunch of loops

                err = fpCreateSwapchainKHR(m_device, &swapchainInfo, nullptr, &m_swapchain);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderer::prepareSwapchainBuffers(): Failed to create Swapchain.\n");
                    return false;
                }

                //Destroy old swapchain
                if (oldSwapchain != VK_NULL_HANDLE)
                    fpDestroySwapchainKHR(m_device, oldSwapchain, nullptr);

                //Get the swapchain images
                uint32_t swapchainImageCount = 0;
                err = fpGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, nullptr);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderer::prepareSwapchainBuffers(): Failed to get swapchain image count.\n");
                    return false;
                }

                std::vector<VkImage> swapchainImages;
                swapchainImages.resize(swapchainImageCount);
                err = fpGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, &swapchainImages[0]);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderer::prepareSwapchainBuffers(): Failed to get swapchain images.\n");
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
                    err = vkCreateImageView(m_device, &colorImageView, nullptr, &buffer.view);

                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderer::prepareSwapchainBuffers(): Failed to create image view.\n");
                        return false;
                    }

                    m_swapchainBuffers.push_back(buffer);
                }

                m_postPresentCommands.resize(m_swapchainBuffers.size());
                m_prePresentCommands.resize(m_swapchainBuffers.size());

                for (int i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    m_swapchainBuffers[i].command = VK_NULL_HANDLE;
                    m_postPresentCommands[i] = VK_NULL_HANDLE;
                    m_prePresentCommands[i] = VK_NULL_HANDLE;
                }

                return true;
            }

            bool VKSwapChain::prepareSwapchainDepth(const VkFormat& preferredDepthFormat, VkExtent2D extent)
            {
                VkResult err;

                VkImageCreateInfo image;
                image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                image.pNext = nullptr;
                image.imageType = VK_IMAGE_TYPE_2D;
                image.format = preferredDepthFormat;
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
                depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                depthSubresourceRange.baseMipLevel = 0;
                depthSubresourceRange.levelCount = 1;
                depthSubresourceRange.baseArrayLayer = 0;
                depthSubresourceRange.layerCount = 1;

                VkImageViewCreateInfo view;
                view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view.pNext = nullptr;
                view.image = VK_NULL_HANDLE;
                view.format = preferredDepthFormat;
                view.subresourceRange = depthSubresourceRange;
                view.flags = 0;
                view.components = components;
                view.viewType = VK_IMAGE_VIEW_TYPE_2D;

                m_depthBuffer.format = preferredDepthFormat;

                VkMemoryRequirements memoryRequirements;
                bool pass;

                //Create image
                err = vkCreateImage(m_device, &image, nullptr, &m_depthBuffer.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSwapchainDepth(): Error, failed to create image\n");
                    return false;
                }

                vkGetImageMemoryRequirements(m_device, m_depthBuffer.image, &memoryRequirements);

                m_depthBuffer.memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                m_depthBuffer.memAllocInfo.pNext = nullptr;
                m_depthBuffer.memAllocInfo.allocationSize = memoryRequirements.size;
                m_depthBuffer.memAllocInfo.memoryTypeIndex = 0;

                //No requirements
                pass = renderer->MemoryTypeFromProperties(memoryRequirements.memoryTypeBits, 0, &m_depthBuffer.memAllocInfo.memoryTypeIndex);
                assert(pass);
                if (!pass)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSwapchainDepth(): Error, failed to get memory type for depth buffer\n");
                    return false;
                }

                //Allocate Memory
                err = vkAllocateMemory(m_device, &m_depthBuffer.memAllocInfo, nullptr, &m_depthBuffer.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSwapchainDepth(): Error, failed to allocate memory for depth buffer\n");
                    return false;
                }

                //Bind Memory
                err = vkBindImageMemory(m_device, m_depthBuffer.image, m_depthBuffer.memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSwapchainDepth(): Error, failed to bind depth buffer memory\n");
                    return false;
                }

                VkCommandBuffer setupCommand = renderer->GetSetupCommandBuffer();
                renderer->SetImageLayout(setupCommand, m_depthBuffer.image, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                   VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

                //Create image view
                view.image = m_depthBuffer.image;
                err = vkCreateImageView(m_device, &view, nullptr, &m_depthBuffer.view);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareSwapchainDepth(): Error, failed create image view for depth buffer\n");
                    return false;
                }

                return true;
            }

            bool VKSwapChain::prepareRenderPass()
            {
                VkAttachmentDescription attachments[2];
                attachments[0].format = m_preferredColorFormat;
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

                err = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::prepareRenderPass(): Failed to create render pass\n");
                    return false;
                }

                return true;
            }

            bool VKSwapChain::prepareFramebuffers(VkExtent2D extents)
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
                    err = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &framebuffer);

                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKSwapChain::prepareFrambuffers(): Failed to create framebuffer at index:%d \n", i);
                        return false;
                    }

                    m_framebuffers.push_back(framebuffer);
                }

                return true;
            }

            bool VKSwapChain::allocateCommandBuffers() 
            {
                VkResult err;

                VkCommandBufferAllocateInfo allocateInfo;
                allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocateInfo.pNext = nullptr;
                allocateInfo.commandPool = m_commandPool;
                allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocateInfo.commandBufferCount = 1;

                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    if (m_swapchainBuffers[i].command != VK_NULL_HANDLE)
                        continue;

                    err = vkAllocateCommandBuffers(m_device, &allocateInfo, &m_swapchainBuffers[i].command);
                    assert(!err);

                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKSwapChain::allocateCommandBuffers(): Failed to allocate for command buffer at index:%d \n", i);
                        return false;
                    }
                }

                //Create command buffers for pre and post barriers
                allocateInfo.commandBufferCount = static_cast<uint32_t>(m_swapchainBuffers.size());

                size_t i;
                bool commandsFree = true;

                for (i = 0; i < m_postPresentCommands.size(); i++)
                {
                    if (m_postPresentCommands[i] != VK_NULL_HANDLE)
                    {
                        commandsFree = false;
                        break;
                    }
                }

                if (commandsFree)
                {
                    err = vkAllocateCommandBuffers(m_device, &allocateInfo, m_postPresentCommands.data());
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKSwapChain::allocateCommandBuffers(): Failed to allocate for post present barrier \n");
                        return false;
                    }
                }

                //Allocate prePresent commands if it won't cause a leak
                commandsFree = true;
                for (i = 0; i < m_prePresentCommands.size(); i++)
                {
                    if (m_prePresentCommands[i] != VK_NULL_HANDLE)
                    {
                        commandsFree = false;
                        break;
                    }
                }

                if (commandsFree)
                {
                    err = vkAllocateCommandBuffers(m_device, &allocateInfo, m_prePresentCommands.data());
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKSwapChain::allocateCommandBuffers(): Failed to allocate for pre present barrier \n");
                        return false;
                    }
                }

                return true;
            }

            bool VKSwapChain::submitBarrier(const VkQueue& queue, const VkCommandBuffer& command)
            {
                VkResult err;

                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &command;

                err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
                if (err != VK_SUCCESS)
                {
                    return false;
                }

                return true;
            }

            void VKSwapChain::destroySurface() 
            {
                vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            }
            void VKSwapChain::destroyPipeline()
            {
                vkFreeMemory(m_device, m_vertexBuffer.memory, nullptr);
                vkDestroyBuffer(m_device, m_vertexBuffer.buffer, nullptr);

                vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_descriptorSet);
            }
            void VKSwapChain::destroyDepth()
            {
                //Destroy depth
                vkDestroyImageView(m_device, m_depthBuffer.view, nullptr);
                vkDestroyImage(m_device, m_depthBuffer.image, nullptr);
                vkFreeMemory(m_device, m_depthBuffer.memory, nullptr);
            }
            void VKSwapChain::destroyFramebuffers()
            {
                //Clear out framebuffers
                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++)
                    vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
                m_framebuffers.clear();
            }
            void VKSwapChain::destroySwapchainBuffers()
            {
                uint32_t bufferCount = static_cast<uint32_t>(m_swapchainBuffers.size());

                for (uint32_t i = 0; i < bufferCount; i++)
                {
                    vkDestroyImageView(m_device, m_swapchainBuffers[i].view, nullptr);
                    //vkDestroyImage(m_device, m_swapchainBuffers[i].image, nullptr); fpDestroySwapchainKHR will also destroy images

                    vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_swapchainBuffers[i].command);
                }

                vkFreeCommandBuffers(m_device, m_commandPool, bufferCount, m_postPresentCommands.data());
                vkFreeCommandBuffers(m_device, m_commandPool, bufferCount, m_prePresentCommands.data());
            }
            void VKSwapChain::destroyRenderPass() 
            {
                vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            }
            void VKSwapChain::destroySwapchain()
            {
                fpDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            }

        }
    }
}
