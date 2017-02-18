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

/**
 * \file ht_vkdevice.cpp
 * \brief VKDevice class implementation
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains implementation for VKDevice class
 */

#include <ht_vkdevice.h>
#include <ht_vkapplication.h>
#include <ht_debug.h>

#include <vector>
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
                vkDestroyDevice(m_vkDevice, nullptr);
            }

            bool VKDevice::Initialize(VKApplication& instance, uint32_t index) {

                if (!EnumeratePhysicalDevices(instance, index))
                    return false;
                
                if (!QueryPhysicalDeviceInfo())
                    return false;

                /*
                * Query information about device queue family
                */
                uint32_t queueCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueCount, nullptr);
                if (queueCount < 1) {
                    HT_ERROR_PRINTF("VKDevice::Initialize(): Invalid queue count.\n");
                    return false;
                }
                m_vkQueueFamilyProperties.resize(queueCount);
                vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueCount, m_vkQueueFamilyProperties.data());

                QueueFamily queueFamily = QueryQueueFamily(VK_QUEUE_GRAPHICS_BIT);

                VkResult err = VK_SUCCESS;

                float QueueProperties[] = { 0.0f };

                VkDeviceQueueCreateInfo queue = {};
                queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue.pNext = nullptr;
                queue.queueFamilyIndex = queueFamily.graphics;
                queue.queueCount = 1;
                queue.pQueuePriorities = QueueProperties;


                std::vector<VkDeviceQueueCreateInfo> queueInfos;
                queueInfos.push_back(queue);

                VkDeviceCreateInfo device = {};
                device.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device.pNext = nullptr;
                device.queueCreateInfoCount = 1;
                device.pQueueCreateInfos = queueInfos.data();
                device.enabledExtensionCount = 1;
                std::vector<const char *> extensions;
                extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
                device.ppEnabledExtensionNames = extensions.data();
                device.pEnabledFeatures = nullptr;
                device.flags = 0;

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

            bool VKDevice::EnumeratePhysicalDevices(VKApplication& instance, uint32_t index)
            {
                VkResult err = VK_SUCCESS;

                uint32_t numDevices = 0;
                err = vkEnumeratePhysicalDevices(static_cast<VkInstance>(instance), &numDevices, nullptr);
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
                err = vkEnumeratePhysicalDevices(static_cast<VkInstance>(instance), &numDevices, devices.data());
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

            VKDevice::QueueFamily VKDevice::QueryQueueFamily(VkQueueFlagBits flags)
            {
                QueueFamily family = {};

                /*Find family index for graphics bit*/
                if (flags & VK_QUEUE_GRAPHICS_BIT)
                {
                    for (uint32_t i = 0; i < m_vkQueueFamilyProperties.size(); i++) {
                        if (m_vkQueueFamilyProperties[i].queueFlags & flags) {
                            family.graphics = i;
                            break;
                        }
                    }
                }


                return family;
            }

            bool VKDevice::GetSupportedDepthFormat(VkFormat& pFormat) const
            {
                std::vector<VkFormat> pFormats =
                {
                    VK_FORMAT_D32_SFLOAT_S8_UINT,
                    VK_FORMAT_D32_SFLOAT,
                    VK_FORMAT_D24_UNORM_S8_UINT,
                    VK_FORMAT_D16_UNORM_S8_UINT,
                    VK_FORMAT_D16_UNORM
                };

                for (auto it = pFormats.begin(); it != pFormats.end(); ++it)
                {
                    VkFormatProperties formatProp;
                    vkGetPhysicalDeviceFormatProperties(m_vkPhysicalDevice, *it, &formatProp);

                    if ( formatProp.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
                    {
                        pFormat = *it;
                        return true;
                    }
                }

                return false;
            }

            uint32_t VKDevice::GetMemoryType(uint32_t pTypeBits, VkMemoryPropertyFlags pProperties, VkBool32 *pMemTypeFound) const
            {
                for (uint32_t i = 0; i < m_vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++ )
                {
                    if ((( pTypeBits & 1) == 1 ) && ( m_vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & pProperties) == pProperties )
                    {
                        if ( pMemTypeFound )
                            *pMemTypeFound = true;

                        return i;
                    }

                    pTypeBits >>= 1;
                }
                
                HT_ERROR_PRINTF("VKDevice::GetMemoryType() Could not find a matching memory type.\n");
                if ( pMemTypeFound )
                    *pMemTypeFound = false;

                return 0;
            }
        }
    }
}
