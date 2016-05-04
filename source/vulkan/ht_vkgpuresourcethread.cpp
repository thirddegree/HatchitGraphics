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

#include <ht_gpuresourcerequest.h>
#include <ht_vkgpuresourcethread.h>
#include <ht_device.h>
#include <ht_vkdevice.h>
#include <ht_debug.h>

#include <ht_texture_resource.h>
#include <ht_material_resource.h>
#include <ht_material.h>
#include <ht_vkmaterial.h>
#include <ht_vkrootlayout.h>
#include <ht_vkrenderpass.h>
#include <ht_vkrendertarget.h>
#include <ht_vkmesh.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKGPUResourceThread::VKGPUResourceThread(VKDevice* device, VKSwapChain* swapchain)
            {
                m_alive = false;
                m_tfinished = false;
                m_locked = false;
                m_device = device;
                m_swapchain = swapchain;

                //auto _device = m_device->GetVKDevices()[0];
                //
                ///*Create thread specific resources*/
                //if (!createCommandPool(_device))
                //{
                //    HT_ERROR_PRINTF("VKGPUResourceThread::thread_main: Failed to create command pool in thread.\n");
                //    Kill();
                //}
                //
                //if (!createDescriptorPool(_device))
                //{
                //    HT_ERROR_PRINTF("VKGPUResourceThread::thread_main: Failed to create descriptor pool in thread.\n");
                //    Kill();
                //}
                //
                //Resource::PipelineHandle handle = Resource::Pipeline::GetHandleFromFileName("SwapchainPipeline.json");
                //VKPipeline* pipeline = new VKPipeline();
                //pipeline->Initialize(handle, _device, m_descriptorPool);

            }

            VKGPUResourceThread::~VKGPUResourceThread()
            {
                Kill();
            }

            void VKGPUResourceThread::VStart()
            {
                m_alive = true;

                m_thread = std::thread(&VKGPUResourceThread::thread_main, this);
            }

            void VKGPUResourceThread::thread_main()
            {
                auto device = m_device->GetVKDevices()[0];

                /*Create thread specific resources*/
                if (!createCommandPool(device))
                {
                    HT_ERROR_PRINTF("VKGPUResourceThread::thread_main: Failed to create command pool in thread.\n");
                    Kill();
                }

                if (!createDescriptorPool(device))
                {
                    HT_ERROR_PRINTF("VKGPUResourceThread::thread_main: Failed to create descriptor pool in thread.\n");
                    Kill();
                }

                while (m_alive)
                {
                    /*Load any resource requests asynchrounsly*/
                    if (m_requests.empty())
                        continue;

                    auto request = m_requests.pop();
                    switch ((*request)->type)
                    {
                        case GPUResourceRequest::Type::Texture:
                        {
                            auto tRequest = static_cast<TextureRequest*>(*request);

                            ProcessTextureRequest(tRequest);
                        } break;

                        case GPUResourceRequest::Type::Material:
                        {
                            auto mRequest = static_cast<MaterialRequest*>(*request);

                            ProcessMaterialRequest(mRequest);
                        } break;

                        case GPUResourceRequest::Type::RootLayout:
                        {
                            auto rRequest = static_cast<RootLayoutRequest*>(*request);

                            ProcessRootLayoutRequest(rRequest);
                        } break;

                        case GPUResourceRequest::Type::Pipeline:
                        {
                            auto pRequest = static_cast<PipelineRequest*>(*request);

                            ProcessPipelineRequest(pRequest);
                        } break;

                        case GPUResourceRequest::Type::Shader:
                        {
                            auto sRequest = static_cast<ShaderRequest*>(*request);

                            ProcessShaderRequest(sRequest);
                        } break;
                    }

                    m_processed = true;
                    m_cv.notify_one();
                }

                vkDestroyCommandPool(device, m_commandPool, nullptr);
                vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
            }

            void VKGPUResourceThread::VCreateTextureBase(Resource::TextureHandle handle, void ** base)
            {
                VKTexture** _base = reinterpret_cast<VKTexture**>(base);
                if (!*_base)
                {
                    *_base = new VKTexture;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0]))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Texture Resource.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreateMaterialBase(Resource::MaterialHandle handle, void ** base)
            {
                VKMaterial** _base = reinterpret_cast<VKMaterial**>(base);
                if (!*_base)
                {
                    *_base = new VKMaterial;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0], m_descriptorPool))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Material Resource.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreateRootLayoutBase(Resource::RootLayoutHandle handle, void ** base)
            {
                VKRootLayout** _base = reinterpret_cast<VKRootLayout**>(base);
                if (!*_base)
                {
                    *_base = new VKRootLayout;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0]))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU RootLayout Resource.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreatePipelineBase(Resource::PipelineHandle handle, void ** base)
            {
                VKPipeline** _base = reinterpret_cast<VKPipeline**>(base);
                if (!*_base)
                {
                    *_base = new VKPipeline;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0], m_descriptorPool))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Pipeline Resource.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreateShaderBase(Resource::ShaderHandle handle, void ** base)
            {
                VKShader** _base = reinterpret_cast<VKShader**>(base);
                if (!*_base)
                {
                    *_base = new VKShader;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0]))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Shader.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreateRenderPassBase(Resource::RenderPassHandle handle, void ** base)
            {
                VKRenderPass** _base = reinterpret_cast<VKRenderPass**>(base);
                if (!*_base)
                {
                    *_base = new VKRenderPass;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0], m_commandPool, m_descriptorPool, m_swapchain))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Render Pass.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreateRenderTargetBase(Resource::RenderTargetHandle handle, void ** base)
            {
                VKRenderTarget** _base = reinterpret_cast<VKRenderTarget**>(base);
                if (!*_base)
                {
                    *_base = new VKRenderTarget;
                    if (!(*_base)->Initialize(handle, m_device->GetVKDevices()[0], m_device->GetVKPhysicalDevices()[0], m_swapchain))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Render Target.\n");
                    }
                }
            }

            void VKGPUResourceThread::VCreateMeshBase(Resource::ModelHandle handle, void ** base)
            {
                VKMesh** _base = reinterpret_cast<VKMesh**>(base);
                if (!*_base)
                {
                    *_base = new VKMesh;
                    if (!(*_base)->Initialize(handle->GetMeshes()[0], m_device->GetVKDevices()[0]))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Mesh.\n");
                    }
                }
            }

            bool VKGPUResourceThread::createCommandPool(const VkDevice& device) 
            {
                VkResult err;

                //Create the command pool
                VkCommandPoolCreateInfo commandPoolInfo;
                commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                commandPoolInfo.pNext = nullptr;
                commandPoolInfo.queueFamilyIndex = 0;
                commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                err = vkCreateCommandPool(device, &commandPoolInfo, nullptr, &m_commandPool);

                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRenderer::setupCommandPool: Error creating command pool.\n");
                    return false;
                }

                return true;
            }
            bool VKGPUResourceThread::createDescriptorPool(const VkDevice& device)
            {
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
        }
    }
}