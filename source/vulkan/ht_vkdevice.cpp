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

#include <ht_vkdevice.h>
#include <ht_vkapplication.h>
#include <ht_debug.h>

#include <algorithm>

namespace Hatchit {
    namespace Graphics {
        namespace Vulkan {

            VKDevice::VKDevice()
            {
                m_vkDevice = VK_NULL_HANDLE;
                m_vkPhysicalDevice = VK_NULL_HANDLE;
            }

            VKDevice::~VKDevice()
            {

            }

            bool VKDevice::Initialize(VKApplication& instance, uint32_t index) {

                if (!EnumeratePhysicalDevices(instance, index))
                    return false;
                
                if (!QueryPhysicalDeviceInfo())
                    return false;

                VkResult err = VK_SUCCESS;

                float QueueProperties[] = { 0.0f };

                VkDeviceQueueCreateInfo queue;
                queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue.pNext = nullptr;
                queue.queueFamilyIndex = 0; //TODO: This could be subject to change
                queue.queueCount = 1;
                queue.pQueuePriorities = QueueProperties;


                VkDeviceCreateInfo device;
                device.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device.pNext = nullptr;
                device.queueCreateInfoCount = 1;
                device.pQueueCreateInfos = &queue;
                device.enabledExtensionCount = instance.EnabledExtensionCount();
                std::vector<const char *> extensions;
                std::transform(instance.EnabledExtensionNames().begin(), instance.EnabledExtensionNames().end(), std::back_inserter(extensions),
                    [](const std::string &s) {
                    return s.c_str();
                });
                extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
                device.ppEnabledExtensionNames = extensions.data();
                device.pEnabledFeatures = nullptr;
                
                err = vkCreateDevice(m_vkPhysicalDevice, &device, nullptr, &m_vkDevice);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKDevice::Initialize() Failed to create device.\n");
                    return false;
                }


                return true;
            }

            const VkPhysicalDeviceProperties& VKDevice::Properties() const {
                return m_vkPhysicalDeviceProperties;
            }

            VKDevice::operator VkDevice()
            {
                return m_vkDevice;
            }

            VKDevice::operator VkPhysicalDevice()
            {
                return m_vkPhysicalDevice;
            }

            bool VKDevice::EnumeratePhysicalDevices(VKApplication & instance, uint32_t index)
            {
                VkResult err = VK_SUCCESS;

                uint32_t numDevices = 0;
                err = vkEnumeratePhysicalDevices(instance, &numDevices, nullptr);
                if (numDevices <= 0 || err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKDevice::EnumeratePhysicalDevices() Vulkan encountered error enumerating devices.\n");
                    return false;
                }

                if (index >= numDevices || index < 0)
                {
                    HT_ERROR_PRINTF("VKDevice::EnumeratePhysicalDevices() Error bad index value\n");
                    return false;
                }

                std::vector<VkPhysicalDevice> devices(numDevices);
                err = vkEnumeratePhysicalDevices(instance, &numDevices, devices.data());
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKDevice::EnumeratePhysicalDevices() Vulkan encountered error enumerating devices.\n");
                    return false;
                }
                m_vkPhysicalDevice = devices[index];

                return true;
            }

            bool VKDevice::QueryPhysicalDeviceInfo()
            {
                if (m_vkPhysicalDevice == VK_NULL_HANDLE)
                    return false;

                /*
                *   Query features of physical device
                *   The returned structure will define all the feature capapbilities
                *   of the device, which can then be queried to make sure the user has
                *   sufficient support for specific software features.
                */
                vkGetPhysicalDeviceFeatures(m_vkPhysicalDevice, &m_vkPhysicalDeviceFeatures);
                vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceProperties);
                vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceMemoryProperties);


                return true;
            }
        }
    }
}