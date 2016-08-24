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

#include <ht_vkqueue.h>
#include <cassert>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKQueue::VKQueue(QueueType queueType) 
            {
                m_queueType = queueType;
                m_queue = VK_NULL_HANDLE;
            }

            VKQueue::~VKQueue() 
            {
                //Don't need to destroy the queue; it goes out with the device
            }

            bool VKQueue::Initialize(const VKDevice* dev)
            {
                VkDevice device = dev->GetVKDevices()[0];
                VkPhysicalDevice gpu = dev->GetVKPhysicalDevices()[0];

                // Find a queue that supports graphics operations
                int32_t graphicsQueueIndex = -1;
                uint32_t queueCount;
                vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, NULL);
                assert(queueCount >= 1);

                std::vector<VkQueueFamilyProperties> queueProps;
                queueProps.resize(queueCount);
                vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, queueProps.data());

                for (uint32_t i = 0; i < queueCount; i++)
                {
                    if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                        m_queueType == QueueType::GRAPHICS)
                    {
                        graphicsQueueIndex = i;
                        break;
                    }
                    else if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
                        m_queueType == QueueType::COMPUTE)
                    {
                        graphicsQueueIndex = i;
                        break;
                    }
                    else if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                        m_queueType == QueueType::COPY)
                    {
                        graphicsQueueIndex = i;
                        break;
                    }
                }
                assert(graphicsQueueIndex >= 0);
                assert(graphicsQueueIndex < static_cast<int32_t>(queueCount));

                // Get the graphics queue
                vkGetDeviceQueue(device, graphicsQueueIndex, 0, &m_queue);

                return true;
            }

            const VkQueue& VKQueue::GetVKQueue() const { return m_queue; }
        }
    }
}