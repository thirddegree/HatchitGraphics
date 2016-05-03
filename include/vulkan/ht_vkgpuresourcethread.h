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
#include <ht_threadqueue.inl>
#include <thread>
#include <atomic>
#include <ht_vulkan.h>
#include <ht_material_resource.h>
#include <ht_texture_resource.h>

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
                VKGPUResourceThread(VKDevice* device);

                ~VKGPUResourceThread();

                void VStart()                                   override;

            private:
                VKDevice*               m_device;

                VkCommandPool m_commandPool;
                VkDescriptorPool m_descriptorPool;

                void ProcessTextureRequest(TextureRequest* request);
                void ProcessMaterialRequest(MaterialRequest* request);

                void CreateTextureBase(Resource::TextureHandle handle, void** base);
                void CreateMaterialBase(Resource::MaterialHandle handle, void** base);

                void thread_main();

                bool createCommandPool(const VkDevice& device);
                bool createDescriptorPool(const VkDevice& device);
            };
        }
    }
}