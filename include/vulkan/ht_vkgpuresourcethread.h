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

#pragma once

#include <ht_platform.h>
#include <ht_gpuresourcerequest.h>
#include <ht_gpuresourcethread.h>
#include <ht_threadvector.h>
#include <ht_threadstack.h>
#include <thread>
#include <atomic>
#include <ht_vulkan.h>
#include <ht_vkswapchain.h>
#include <ht_material_resource.h>
#include <ht_texture_resource.h>
#include <ht_model.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;

            class HT_API VKGPUResourceThread : public GPUResourceThread
            {
                using GPURequestQueue = Core::ThreadsafeQueue<GPUResourceRequest>;
            public:
                VKGPUResourceThread(VKDevice* device, VKSwapChain* swapchain);

                ~VKGPUResourceThread();

                void VStart()                                   override;

            private:
                VKDevice*   m_device;
                VKSwapChain* m_swapchain;

                VkCommandPool       m_commandPool;
                VkDescriptorPool    m_descriptorPool;

                void VCreateTextureBase(Resource::TextureHandle handle, void** base)            override;
                void VCreateMaterialBase(Resource::MaterialHandle handle, void** base)          override;
                void VCreateRootLayoutBase(Resource::RootLayoutHandle handle, void** base)      override;
                void VCreatePipelineBase(Resource::PipelineHandle handle, void** base)          override;
                void VCreateShaderBase(Resource::ShaderHandle handle, void** base)              override;
                void VCreateRenderPassBase(Resource::RenderPassHandle handle, void** base)      override;
                void VCreateRenderTargetBase(Resource::RenderTargetHandle handle, void** base)  override;
                void VCreateMeshBase(Resource::ModelHandle handle, void** base)                 override;

                void thread_main();

                bool createCommandPool(const VkDevice& device);
                bool createDescriptorPool(const VkDevice& device);
            };
        }
    }
}