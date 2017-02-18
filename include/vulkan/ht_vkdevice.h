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

/**
 * \file ht_vkdevice.h
 * \brief VKDevice class definition
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKDevice class
 */

#include <ht_platform.h>
#include <ht_string.h>
#include <ht_vulkan.h>
#include <set>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKApplication;

            /**
             * \class VKDevice
             * \brief Vulkan device wrapper
             *
             * This class wraps the functionality associated with interfacing with a GPU device
             * using Vulkan. Since there can be multiple active devices, this class represents a single
             * device instance.
             */
            class HT_API VKDevice
            {
                struct QueueFamily
                {
                    uint32_t graphics;
                    uint32_t compute;
                };
            public:
                VKDevice();

                ~VKDevice();

                bool Initialize(VKApplication& instance, uint32_t index);

                const VkPhysicalDeviceProperties& Properties() const;

                /* Searches for available depth formats on the device based on the precision
                *  then sets it on pFormat, return true if formar found, false otherwise.
                */
                bool GetSupportedDepthFormat(VkFormat& pFormat) const;

                /* Get the type memoty index considering the memory property flags */
                uint32_t GetMemoryType(uint32_t pTypeBits, VkMemoryPropertyFlags pProperties, VkBool32* pMemTypeFound = nullptr) const;

                operator VkDevice();
                operator VkPhysicalDevice();

            private:
                VkDevice                            m_vkDevice;
                VkPhysicalDevice                    m_vkPhysicalDevice;
                VkPhysicalDeviceFeatures            m_vkPhysicalDeviceFeatures;
                VkPhysicalDeviceProperties          m_vkPhysicalDeviceProperties;
                VkPhysicalDeviceMemoryProperties    m_vkPhysicalDeviceMemoryProperties;

                std::vector<VkQueueFamilyProperties> m_vkQueueFamilyProperties;

                bool EnumeratePhysicalDevices(VKApplication& instance, uint32_t index);
                bool QueryPhysicalDeviceInfo();
                QueueFamily QueryQueueFamily(VkQueueFlagBits flags);
            };
        }
    }
}
