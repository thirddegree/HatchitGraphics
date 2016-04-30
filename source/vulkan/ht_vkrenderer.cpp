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

#include <ht_vkmesh.h>
#include <ht_vkrenderer.h>
#include <ht_vkshader.h>
#include <ht_vkpipeline.h>
#include <ht_vkmaterial.h>
#include <ht_vksampler.h>
#include <ht_vktexture.h>
#include <ht_vkrendertarget.h>
#include <ht_debug.h>
#include <ht_scheduler.h>

#include <cassert>

#ifdef HT_SYS_WINDOWS
#include <windows.h>
#endif

#ifdef HT_SYS_LINUX
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

//#define NO_VALIDATION

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Hatchit::Resource;

            VKRenderer::VKRenderer()
            {
                m_setupCommandBuffer = 0;

                m_swapchain = nullptr;

                m_commandPool = VK_NULL_HANDLE;
                m_descriptorPool = VK_NULL_HANDLE;

                msg_callback = VK_NULL_HANDLE;

                m_renderSemaphore = VK_NULL_HANDLE;
                m_presentSemaphore = VK_NULL_HANDLE;

                m_enableValidation = false;
            }

            VKRenderer::~VKRenderer()
            {
            }

            bool VKRenderer::VInitialize(const RendererParams & params)
            {
                //Store params for later
                m_rendererParams = params;

#ifndef NO_VALIDATION
                m_enableValidation = params.validate;
#endif

                m_clearColor.color.float32[0] = params.clearColor.r;
                m_clearColor.color.float32[1] = params.clearColor.g;
                m_clearColor.color.float32[2] = params.clearColor.b;
                m_clearColor.color.float32[3] = params.clearColor.a;

                /*
                * Create core VKDevice
                */
                m_device.SetValidation(m_enableValidation);
                m_device.VInitialize();

                /*
                * Setup debug callbacks
                */
#ifdef _DEBUG
                if (!setupDebugCallbacks())
                    return false;
#endif

                /*
                * Initialize Vulkan swapchain
                */
                if (!initVulkanSwapchain())
                    return false;

                if (!setupCommandPool())
                    return false;

                if (!setupDescriptorPool())
                    return false;

                /*
                * Prepare Vulkan command buffers and memory systems for drawing
                */
                if (!prepareVulkan())
                    return false;

                //TODO: remove this test code
                m_rootLayout = VKRootLayout::GetHandle("TestRootDescriptor.json", "TestRootDescriptor.json", &m_device.GetVKDevices()[0]);

                ModelHandle model = Model::GetHandleFromFileName("Raptor.obj");
                ModelHandle lightModel = Model::GetHandleFromFileName("IcoSphere.dae");
                ModelHandle fullscreenTri = Model::GetHandleFromFileName("Tri.obj");

                m_renderPass = VKRenderPass::GetHandle("DeferredPass.json", "DeferredPass.json", this);
                m_lightingPass = VKRenderPass::GetHandle("LightingPass.json", "LightingPass.json", this);
                m_compositionPass = VKRenderPass::GetHandle("CompositionPass.json", "CompositionPass.json", this);
                
                CreateSetupCommandBuffer();
                m_texture = VKTexture::GetHandle("raptor.png", "raptor.png", this).StaticCastHandle<Texture>();

                m_pipeline = VKPipeline::GetHandle("DeferredPipeline.json", "DeferredPipeline.json", this).StaticCastHandle<IPipeline>();
                m_pipeline->VUpdate();
                m_pointLightingPipeline = VKPipeline::GetHandle("PointLightingPipeline.json", "PointLightingPipeline.json", this).StaticCastHandle<IPipeline>();
                m_compositionPipeline = VKPipeline::GetHandle("CompositionPipeline.json", "CompositionPipeline.json", this).StaticCastHandle<IPipeline>();

                m_material = Material::GetHandle("DeferredMaterial.json", "DeferredMaterial.json");
                m_pointLightMaterial = Material::GetHandle("PointLightMaterial.json", "PointLightMaterial.json");
                m_compositionMaterial = Material::GetHandle("CompositionMaterial.json", "CompositionMaterial.json");

                m_meshHandle = VKMesh::GetHandle("raptor", model->GetMeshes()[0], this).StaticCastHandle<IMesh>();
                m_pointLightMeshHandle = VKMesh::GetHandle("pointLight", lightModel->GetMeshes()[0], this).StaticCastHandle<IMesh>();
                m_compositionMeshHandle = VKMesh::GetHandle("pointLight", fullscreenTri->GetMeshes()[0], this).StaticCastHandle<IMesh>();

                RegisterRenderPass(m_renderPass.StaticCastHandle<RenderPassBase>());
                RegisterRenderPass(m_lightingPass.StaticCastHandle<RenderPassBase>());
                RegisterRenderPass(m_compositionPass.StaticCastHandle<RenderPassBase>());

                m_swapchain->VKSetIncomingRenderPass(m_compositionPass);

                m_swapchain->VKPrepareResources();

                FlushSetupCommandBuffer();

                return true;
            }

            void VKRenderer::VDeInitialize()
            {
                m_queueProps.clear();

                if (m_swapchain != nullptr)
                    delete m_swapchain;

                for (int i = 0; i < m_renderPassLayers.size(); i++)
                {
                    m_renderPassLayers[i].clear();
                }

                m_rootLayout.Release();

                m_material.Release();
                m_texture.Release();
                m_pipeline.Release();
                m_meshHandle.Release();
                m_renderPass.Release();

                m_lightingPass.Release();
                m_pointLightingPipeline.Release();
                m_pointLightMaterial.Release();
                m_pointLightMeshHandle.Release();

                m_compositionPass.Release();
                m_compositionPipeline.Release();
                m_compositionMaterial.Release();
                m_compositionMeshHandle.Release();

                VkInstance instance = m_device.GetVKInstance();
                VkDevice device = m_device.GetVKDevices()[0];

                if (device != VK_NULL_HANDLE)
                {
                    if (m_presentSemaphore != VK_NULL_HANDLE)
                        vkDestroySemaphore(device, m_presentSemaphore, nullptr);
                    if (m_renderSemaphore != VK_NULL_HANDLE)
                        vkDestroySemaphore(device, m_renderSemaphore, nullptr);

                    if (m_commandPool != VK_NULL_HANDLE)
                        vkDestroyCommandPool(device, m_commandPool, nullptr);
                    if (m_descriptorPool != VK_NULL_HANDLE)
                        vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);

                    vkDestroyDevice(device, nullptr);
                }

                if (instance != VK_NULL_HANDLE)
                {
                    if(msg_callback != VK_NULL_HANDLE)
                        m_destroyDebugReportCallback(instance, msg_callback, nullptr);

                    vkDestroyInstance(instance, nullptr);
                }
            }

            void VKRenderer::VResizeBuffers(uint32_t width, uint32_t height)
            {
                //Recreate the swapchain
                m_width = width;
                m_height = height;

                VkResult err;

                err = vkQueueWaitIdle(m_queue);
                assert(!err);

                //re-prepare the swapchain
                prepareVulkan();

                err = vkQueueWaitIdle(m_queue);
                assert(!err);
            }

            void VKRenderer::VSetClearColor(const Color & color)
            {
                
            }

            void VKRenderer::VClearBuffer(ClearArgs args)
            {
                VkResult err;
                
                VkFence nullFence = VK_NULL_HANDLE;

                //Get the next image to draw on
                //TODO: Actually use fences
                err = m_swapchain->VKGetNextImage(m_presentSemaphore);
                if (err == VK_ERROR_OUT_OF_DATE_KHR)
                {
                    //Resize!
                    VResizeBuffers(m_width, m_height); //TODO: find a better way to resize
                    return;
                }
                else if (err == VK_SUBOPTIMAL_KHR) 
                {
                    // demo->swapchain is not as optimal as it could be, but the platform's
                    // presentation engine will still present the image correctly.
                }
                else
                {
                    assert(!err);
                }
            }

            void VKRenderer::VRender(float dt) 
            {
                //TODO: Remove this
                Math::Matrix4 view = Math::MMMatrixLookAt(Math::Vector3(5, 15, 5), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
                Math::Matrix4 proj = Math::MMMatrixPerspProj(3.14f * 0.25f, static_cast<float>(m_width), static_cast<float>(m_height), 0.1f, 100.0f);

                m_renderPass->VSetView(view);
                m_renderPass->VSetProj(proj);

                m_lightingPass->VSetView(view);
                m_lightingPass->VSetProj(proj);

                Math::Matrix4 scale = Math::MMMatrixScale(Math::Vector3(1.0f, 1.0f, 1.0f));
                Math::Matrix4 rot = Math::MMMatrixRotationXYZ(Math::Vector3(0, m_angle += dt, 0));

                //Make a shit ton of Raptors
                size_t xCount = 10;
                size_t zCount = 10;

                float startingX = (xCount * 0.5f) * -3.0f;
                float startingZ = (zCount * 0.5f) * -3.0f;

                //so we can clean up
                std::vector<std::vector<Resource::ShaderVariable*>> allInstanceVars;

                for (size_t x = 0; x < xCount; x++)
                {
                    for (size_t z = 0; z < zCount; z++)
                    {
                        float xPos = startingX + (x * 3.0f);
                        float zPos = startingZ + (z * 3.0f);

                        Math::Matrix4 translation = Math::MMMatrixTranslation(Math::Vector3(xPos, 0, zPos));

                        Math::Matrix4 modelMatrix = MMMatrixTranspose(translation * scale * rot);

                        std::vector<Resource::ShaderVariable*> instanceVars;
                        instanceVars.push_back(new Resource::Matrix4Variable(modelMatrix));

                        allInstanceVars.push_back(instanceVars);

                        m_renderPass->VScheduleRenderRequest(m_material, m_meshHandle, instanceVars);
                    }
                }

                std::vector<Resource::ShaderVariable*> redLightInstanceVars;
                Math::Matrix4 redLightMat = Math::MMMatrixTranspose(Math::MMMatrixTranslation(Math::Vector3(0, 5, -1)));
                Math::Vector4 redLightColor = Math::Vector4(0.7f, .2f, .2f, 1);
                float redLightRadius = 5.0f;
                Math::Vector3 redLightAttenuation = Math::Vector3(0, 1, 0);

                redLightInstanceVars.push_back(new Resource::Matrix4Variable(redLightMat));
                redLightInstanceVars.push_back(new Resource::Float4Variable(redLightColor));
                redLightInstanceVars.push_back(new Resource::FloatVariable(redLightRadius));
                redLightInstanceVars.push_back(new Resource::Float3Variable(redLightAttenuation));

                std::vector<Resource::ShaderVariable*> blueLightInstanceVars;
                Math::Matrix4 blueLightMat = Math::MMMatrixTranspose(Math::MMMatrixTranslation(Math::Vector3(1, 5, 1)));
                Math::Vector4 blueLightColor = Math::Vector4(0.2f, .2f, .7f, 1);
                float blueLightRadius = 5.0f;
                Math::Vector3 blueLightAttenuation = Math::Vector3(0, 1, 0);

                blueLightInstanceVars.push_back(new Resource::Matrix4Variable(blueLightMat));
                blueLightInstanceVars.push_back(new Resource::Float4Variable(blueLightColor));
                blueLightInstanceVars.push_back(new Resource::FloatVariable(blueLightRadius));
                blueLightInstanceVars.push_back(new Resource::Float3Variable(blueLightAttenuation));

                std::vector<Resource::ShaderVariable*> greenLightInstanceVars;
                Math::Matrix4 greenLightMat = Math::MMMatrixTranspose(Math::MMMatrixTranslation(Math::Vector3(-1, 5, 1)));
                Math::Vector4 greenLightColor = Math::Vector4(0.2f, .7f, .2f, 1);
                float greenLightRadius = 5.0f;
                Math::Vector3 greenLightAttenuation = Math::Vector3(0, 1, 0);

                greenLightInstanceVars.push_back(new Resource::Matrix4Variable(greenLightMat));
                greenLightInstanceVars.push_back(new Resource::Float4Variable(greenLightColor));
                greenLightInstanceVars.push_back(new Resource::FloatVariable(greenLightRadius));
                greenLightInstanceVars.push_back(new Resource::Float3Variable(greenLightAttenuation));

                m_lightingPass->VScheduleRenderRequest(m_pointLightMaterial, m_pointLightMeshHandle, redLightInstanceVars);
                m_lightingPass->VScheduleRenderRequest(m_pointLightMaterial, m_pointLightMeshHandle, blueLightInstanceVars);
                m_lightingPass->VScheduleRenderRequest(m_pointLightMaterial, m_pointLightMeshHandle, greenLightInstanceVars);

                m_compositionPass->VScheduleRenderRequest(m_compositionMaterial, m_compositionMeshHandle, std::vector<Resource::ShaderVariable*>());

                //TODO: Determine which physical device and thread are best to render with

                m_swapchain->BuildSwapchainCommands(m_clearColor);

                bool success = m_swapchain->VKPostPresentBarrier(m_queue);
                assert(success);

                //Get list of command buffers
                std::vector<VkCommandBuffer> commandBuffers;

                for (size_t i = 0; i < m_renderPassLayers[0].size(); i++)
                {
                    VKRenderPassHandle renderPass = m_renderPassLayers[0][i].DynamicCastHandle<VKRenderPass>();

                    renderPass->VBuildCommandList();
                    commandBuffers.push_back(renderPass->GetVkCommandBuffer());
                }

                //Make sure we run the swapchain command
                commandBuffers.push_back(m_swapchain->VKGetCurrentCommand());

                VkResult err;

                m_submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
                m_submitInfo.pCommandBuffers = commandBuffers.data();

                err = vkQueueSubmit(m_queue, 1, &m_submitInfo, VK_NULL_HANDLE);
                assert(!err);

                success = m_swapchain->VKPrePresentBarrier(m_queue);
                assert(success);

                //TODO: remove
                for (size_t i = 0; i < allInstanceVars.size(); i++)
                {
                    std::vector<ShaderVariable*> instanceVars = allInstanceVars[i];
                    for (size_t j = 0; j < instanceVars.size(); j++)
                        delete instanceVars[j];
                }
                for (size_t i = 0; i < redLightInstanceVars.size(); i++)
                {
                    delete redLightInstanceVars[i];
                    delete blueLightInstanceVars[i];
                    delete greenLightInstanceVars[i];
                }
            }

            void VKRenderer::VPresent()
            {
                VkResult err;

                err = m_swapchain->VKPresent(m_queue, m_renderSemaphore);
                if (err == VK_ERROR_OUT_OF_DATE_KHR)
                    VResizeBuffers(m_width, m_height);
                else if (err == VK_SUBOPTIMAL_KHR)
                {
                    //Swapchain is not as optimal as it could be
                }
                else
                    assert(!err);

                err = vkQueueWaitIdle(m_queue);
                assert(!err);

                //Reset command buffer memory back to this command pool
                //vkResetCommandPool(m_device, m_commandPool, 0);
            }

            const VKDevice& VKRenderer::GetVKDevice() const
            {
                return m_device;
            }


            const VkCommandPool& VKRenderer::GetVKCommandPool() const
            {
                return m_commandPool;
            }

            const VkDescriptorPool& VKRenderer::GetVKDescriptorPool() const
            {
                return m_descriptorPool;
            }

            const VKRootLayoutHandle& VKRenderer::GetVKRootLayoutHandle() const
            {
                return m_rootLayout;
            }

            const VkCommandBuffer& VKRenderer::GetSetupCommandBuffer() const
            {
                return m_setupCommandBuffer;
            }

            const VkFormat& VKRenderer::GetPreferredImageFormat() const
            {
                return m_swapchain->VKGetPreferredColorFormat();
            }
            const VkFormat& VKRenderer::GetPreferredDepthFormat() const
            {
                return m_swapchain->VKGetPreferredDepthFormat();
            }

            const RendererParams& VKRenderer::GetRendererParams() const
            {
                return m_rendererParams;
            }

            const VkClearValue& VKRenderer::GetClearColor() const
            {
                return m_clearColor; 
            }

            bool VKRenderer::initVulkanSwapchain()
            {
                VkInstance instance = m_device.GetVKInstance();
                VkPhysicalDevice gpu = m_device.GetVKPhysicalDevices()[0];
                VkDevice device = m_device.GetVKDevices()[0];

                m_swapchain = new VKSwapchain(this, instance, gpu, device, m_commandPool);

                const VkSurfaceKHR& surface = m_swapchain->VKGetSurface();

                /*
                * Create the device object that is in charge of allocating memory and making draw calls
                */
                if (!setupSwapchainFunctions())
                    return false;                

                //Get Device queue
                vkGetDeviceQueue(device, m_swapchain->VKGetGraphicsQueueIndex(), 0, &m_queue);

                VkResult err;

                //Get memory information
                vkGetPhysicalDeviceMemoryProperties(gpu, &m_memoryProps);

                //Setup semaphores and submission info
                VkSemaphoreCreateInfo semaphoreCreateInfo = {};
                semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                semaphoreCreateInfo.pNext = nullptr;
                semaphoreCreateInfo.flags = 0;

                err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore);
                assert(!err);

                err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore);
                assert(!err);

                VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

                m_submitInfo = {};
                m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                m_submitInfo.pWaitDstStageMask = &stageFlags;
                m_submitInfo.waitSemaphoreCount = 1;
                m_submitInfo.pWaitSemaphores = &m_presentSemaphore;
                m_submitInfo.signalSemaphoreCount = 1;
                m_submitInfo.pSignalSemaphores = &m_renderSemaphore;

                return true;
            }

            bool VKRenderer::CreateBuffer(VkDevice device, VkBufferUsageFlagBits usage, size_t dataSize, void* data, UniformBlock_vk* uniformBlock)
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = usage;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &uniformBlock->buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to create buffer\n");
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(device, uniformBlock->buffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to get memory type\n");
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, &uniformBlock->memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to allocate memory\n");
                    return false;
                }

                //We may not ask for a buffer that has anything in it
                if (data != nullptr)
                {
                    err = vkMapMemory(device, uniformBlock->memory, 0, dataSize, 0, &pData);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to map memory\n");
                        return false;
                    }

                    //Actually copy data into location
                    memcpy(pData, data, dataSize);

                    //Unmap memory and then bind to the uniform
                    vkUnmapMemory(device, uniformBlock->memory);
                }

                err = vkBindBufferMemory(device, uniformBlock->buffer, uniformBlock->memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::VBuffer(): Failed to bind memory\n");
                    return false;
                }

                uniformBlock->descriptor.buffer = uniformBlock->buffer;
                uniformBlock->descriptor.offset = 0;
                uniformBlock->descriptor.range = dataSize;

                return true;
            }
            
            bool VKRenderer::setupDebugCallbacks()
            {
                VkInstance instance = m_device.GetVKInstance();

                //Skip this if we don't want to validate
                if (!m_enableValidation)
                    return true;

                VkResult err;

                //Get debug callback function pointers
                m_createDebugReportCallback =
                    (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
                m_destroyDebugReportCallback =
                    (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

                if (!m_createDebugReportCallback)
                {
                    HT_DEBUG_PRINTF("GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n");
                    return false;
                }
                if (!m_destroyDebugReportCallback) {
                    HT_DEBUG_PRINTF("GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n");
                    return false;
                }

                m_debugReportMessage =
                    (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");
                if (!m_debugReportMessage) {
                    HT_DEBUG_PRINTF("GetProcAddr: Unable to find vkDebugReportMessageEXT\n");
                    return false;
                }

                PFN_vkDebugReportCallbackEXT callback;
                callback = VKRenderer::debugFunction;

                VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
                dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
                dbgCreateInfo.pNext = NULL;
                dbgCreateInfo.pfnCallback = callback;
                dbgCreateInfo.pUserData = NULL;
                dbgCreateInfo.flags =
                    VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
                err = m_createDebugReportCallback(instance, &dbgCreateInfo, NULL,
                    &msg_callback);
                switch (err) {
                case VK_SUCCESS:
                    break;
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                    HT_DEBUG_PRINTF("ERROR: Out of host memory!\n");
                    return false;
                default:
                    HT_DEBUG_PRINTF("ERROR: An unknown error occured!\n");
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupSwapchainFunctions()
            {
                VkInstance instance = m_device.GetVKInstance();
                VkDevice device = m_device.GetVKDevices()[0];

                //Pointer to function to get function pointers from device
                PFN_vkGetDeviceProcAddr g_gdpa = (PFN_vkGetDeviceProcAddr)
                    vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr");

                fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(device, "vkCreateSwapchainKHR");
                fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(device, "vkDestroySwapchainKHR");
                fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(device, "vkGetSwapchainImagesKHR");
                fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(device, "vkAcquireNextImageKHR");
                fpQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(device, "vkQueuePresentKHR");

                return true;
            }

            bool VKRenderer::setupCommandPool() 
            {
                VkDevice device = m_device.GetVKDevices()[0];

                VkResult err;

                //Create the command pool
                VkCommandPoolCreateInfo commandPoolInfo;
                commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                commandPoolInfo.pNext = nullptr;
                commandPoolInfo.queueFamilyIndex = m_swapchain->VKGetGraphicsQueueIndex();
                commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                err = vkCreateCommandPool(device, &commandPoolInfo, nullptr, &m_commandPool);

                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderer::setupCommandPool: Error creating command pool.\n");
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupDescriptorPool() 
            {
                VkDevice device = m_device.GetVKDevices()[0];

                VkResult err;

                //Setup the descriptor pool
                std::vector<VkDescriptorPoolSize> poolSizes;

                VkDescriptorPoolSize uniformSize = {};
                uniformSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformSize.descriptorCount = 10;

                VkDescriptorPoolSize imageSize = {};
                imageSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                imageSize.descriptorCount = 10;

                VkDescriptorPoolSize samplerSize = {};
                samplerSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
                samplerSize.descriptorCount = 6;

                poolSizes.push_back(uniformSize);
                poolSizes.push_back(imageSize);
                poolSizes.push_back(samplerSize);

                VkDescriptorPoolCreateInfo poolCreateInfo = {};
                poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolCreateInfo.pPoolSizes = poolSizes.data();
                poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
                poolCreateInfo.maxSets = 16;
                poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

                err = vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &m_descriptorPool);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderer::setupDescriptorPool: Failed to create descriptor pool\n");
                    return false;
                }

                return true;
            }

            //TODO: Move this functionality to other subclasses
            bool VKRenderer::prepareVulkan()
            {
                CreateSetupCommandBuffer();

                m_swapchain->VKPrepare();

                m_width = m_swapchain->GetWidth();
                m_height = m_swapchain->GetHeight();

                FlushSetupCommandBuffer();

                return true;
            }
        
            void VKRenderer::CreateSetupCommandBuffer() 
            {
                VkDevice device = m_device.GetVKDevices()[0];

                if (m_setupCommandBuffer != VK_NULL_HANDLE)
                {
                    vkFreeCommandBuffers(device, m_commandPool, 1, &m_setupCommandBuffer);
                    m_setupCommandBuffer = VK_NULL_HANDLE;
                }

                VkResult err;

                //Start up a basic command buffer if we don't have one already
                if (m_setupCommandBuffer == VK_NULL_HANDLE)
                {
                    VkCommandBufferAllocateInfo command;
                    command.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    command.pNext = nullptr;
                    command.commandPool = m_commandPool;
                    command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    command.commandBufferCount = 1;

                    err = vkAllocateCommandBuffers(device, &command, &m_setupCommandBuffer);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderer::CreateSetupCommandBuffer(): Failed to allocate command buffer.\n");
                    }
                }

                VkCommandBufferBeginInfo cmdBufInfo = {};
                cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                err = vkBeginCommandBuffer(m_setupCommandBuffer, &cmdBufInfo);
                assert(!err);
            }

            void VKRenderer::FlushSetupCommandBuffer()
            {
                VkDevice device = m_device.GetVKDevices()[0];

                VkResult err;
                if (m_setupCommandBuffer == VK_NULL_HANDLE)
                    return;

                err = vkEndCommandBuffer(m_setupCommandBuffer);
                assert(!err);

                VkFence nullFence = VK_NULL_HANDLE;

                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.waitSemaphoreCount = 0;
                submitInfo.pWaitSemaphores = nullptr;
                submitInfo.pWaitDstStageMask = nullptr;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &m_setupCommandBuffer;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;

                err = vkQueueSubmit(m_queue, 1, &submitInfo, nullFence);
                assert(!err);

                err = vkQueueWaitIdle(m_queue);
                assert(!err);

                vkFreeCommandBuffers(device, m_commandPool, 1, &m_setupCommandBuffer);
                m_setupCommandBuffer = VK_NULL_HANDLE;
            }

            bool VKRenderer::SetImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask,
                VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
            {
                VkImageMemoryBarrier imageMemoryBarrier;
                imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageMemoryBarrier.pNext = nullptr;
                imageMemoryBarrier.srcAccessMask = 0;
                imageMemoryBarrier.dstAccessMask = 0;
                imageMemoryBarrier.oldLayout = oldImageLayout;
                imageMemoryBarrier.newLayout = newImageLayout;
                imageMemoryBarrier.image = image;
                imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
                imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
                imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
                imageMemoryBarrier.subresourceRange.layerCount = 1;
                imageMemoryBarrier.subresourceRange.levelCount = 1;
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                // Undefined layout
                // Only allowed as initial layout!
                // Make sure any writes to the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                {
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // Old layout is color attachment
                // Make sure any writes to the color buffer have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                }

                // Old layout is transfer source
                // Make sure any reads from the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // Old layout is shader read (sampler, input attachment)
                // Make sure any shader reads from the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                }

                // Target layouts (new)

                // New layout is transfer destination (copy, blit)
                // Make sure any copyies to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // New layout is transfer source (copy, blit)
                // Make sure any reads from and writes to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                // New layout is color attachment
                // Make sure any writes to the color buffer hav been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                // New layout is depth attachment
                // Make sure any writes to depth/stencil buffer have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                }

                // New layout is shader read (sampler, input attachment)
                // Make sure any writes to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                {
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                }

                if (newImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                }

                // Put barrier on top
                VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

                vkCmdPipelineBarrier(commandBuffer, srcStageFlags, destStageFlags, 0, 0, nullptr, 0,
                    nullptr, 1, &imageMemoryBarrier);

                return true;
            }

            bool VKRenderer::MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex)
            {
                VkPhysicalDeviceMemoryProperties memoryProps = m_memoryProps;

                //Search mem types to find the first index with those properties
                for (uint32_t i = 0; i < 32; i++)
                {
                    if ((typeBits & 1) == 1)
                    {
                        //Type exists; does it match properties we're looking for?
                        if ((memoryProps.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
                        {
                            *typeIndex = i;
                            return true;
                        }
                    }
                    typeBits >>= 1;
                }
                
                return false; //nothing found
            }

            VKAPI_ATTR VkBool32 VKAPI_CALL VKRenderer::debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                uint64_t srcObject, size_t location, int32_t msgCode,
                const char *pLayerPrefix, const char *pMsg, void *pUserData)
            {
                if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
                {
                    HT_ERROR_PRINTF("ERROR: [%s] Code %d : %s\n", pLayerPrefix, msgCode,pMsg);
                }
                else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
                {
                    // We know that we're submitting queues without fences, ignore this
                    // warning
                    if (strstr(pMsg,
                        "vkQueueSubmit parameter, VkFence fence, is null pointer"))
                    {
                        return false;
                    }

                    HT_WARNING_PRINTF("WARNING: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
                }
                else {
                    return false;
                }

                /*
                * false indicates that layer should not bail-out of an
                * API call that had validation failures. This may mean that the
                * app dies inside the driver due to invalid parameter(s).
                * That's what would happen without validation layers, so we'll
                * keep that behavior here.
                */
                return false;
            }
        }
    }
}
