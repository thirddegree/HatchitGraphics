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

#include <ht_vkgpuresourcethread.h>
#include <ht_device.h>
#include <ht_vkdevice.h>
#include <ht_debug.h>

#include <ht_texture_resource.h>
#include <ht_material_resource.h>
#include <ht_material.h>
#include <ht_vkmaterial.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKGPUResourceThread::VKGPUResourceThread(VKDevice* device)
            {
                m_alive = false;
                m_tfinished = false;
                m_device = device;
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
                VkResult err;

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

                    }

                    m_processed = true;
                    m_cv.notify_one();
                }

                vkDestroyCommandPool(device, m_commandPool, nullptr);
                vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
            }

            void VKGPUResourceThread::ProcessTextureRequest(TextureRequest * request)
            {
                Resource::TextureHandle handle = Resource::Texture::GetHandle(request->file, request->file);
                if (!m_locked)
                {
                    HT_DEBUG_PRINTF("Async load.\n");

                }
                else
                {
                    HT_DEBUG_PRINTF("Non-Async load.\n");

                    CreateTextureBase(handle, request->data);
                }
            }

            void VKGPUResourceThread::ProcessMaterialRequest(MaterialRequest * request)
            {
                Resource::MaterialHandle handle = Resource::Material::GetHandle(request->file, request->file);
                if (!m_locked)
                {
                    HT_DEBUG_PRINTF("Async load.\n");

                }
                else
                {
                    HT_DEBUG_PRINTF("Non-Async load.\n");

                    CreateMaterialBase(handle, request->data);
                }
            }

            void VKGPUResourceThread::CreateTextureBase(Resource::TextureHandle handle, void ** base)
            {
                VKTexture** _base = reinterpret_cast<VKTexture**>(base);
                if (!*_base)
                {
                    *_base = new VKTexture;
                    if (!(*_base)->Initialize(handle, m_device))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Texture Resource.\n");
                    }
                }
            }

            void VKGPUResourceThread::CreateMaterialBase(Resource::MaterialHandle handle, void ** base)
            {
                VKTexture** _base = reinterpret_cast<VKTexture**>(base);
                if (!*_base)
                {
                    *_base = new VKTexture;
                    if (!(*_base)->Initialize(handle, m_device))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Material Resource.\n");
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