/**
**    Hatchit Engine
**    Copyright(c) 2015-2017 Third-Degree
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
 * @file ht_vkdevice.h
 * @brief VKDevice class definition
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKDevice class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKApplication;

            /**
             * @class VKDevice
             * @brief Vulkan device wrapper
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

                /**
                 * @brief Constructs device
                 */
                VKDevice();

                /**
                 * @brief Destructs device
                 *
                 * Destroys underlying Vulkan logical device
                 */
                ~VKDevice();


                /**
                 * @brief Initialize Vulkan logical device
                 * @param instance Vulkan application instance
                 * @param index Physical device index
                 * @return [true] : success
                 * @return [false] : failure
                 */
                bool Initialize(VKApplication& instance, uint32_t index);

                /**
                 * @brief Gets physical device properties
                 * @return physical device properties
                 */
                const VkPhysicalDeviceProperties& GetProperties() const;

                /**Searches for available depth formats on the device based on the precision
                *  then sets it on pFormat, return true if formar found, false otherwise.
                */

                /**
                 * @brief Gets supported depth format
                 *
                 * Searches for available depth formats on the device based
                 * on the precision and then sets the result in format.
                 *
                 * @param format Vulkan format result
                 * @return [true] : found
                 * @return [false] : not found
                 */
                bool GetSupportedDepthFormat(VkFormat& format) const;

                /* Get the type memoty index considering the memory property flags */

                /**
                 * @brief Gets the memory type index
                 *
                 * Gets the memory type index considering the memory property
                 * flags provided
                 *
                 * @param bits Specific bit to check
                 * @param properties Properties to constrain search
                 * @param memTypeFound Optional flag pointer set to true if memory index found.
                 * @return index of memory type
                 */
                uint32_t GetMemoryType(uint32_t bits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;

                /**
                 * @brief Gets queue count of logical device
                 * @return queue count
                 */
                uint32_t GetQueueCount();

                /**
                 * @brief Gets queue family index information
                 * @param flags Queue family bits to check
                 * @return queue family with indices
                 */
                QueueFamily GetQueueFamily(VkQueueFlagBits flags);

                /**
                 * @brief Conversion operator to VkDevice
                 * @return underlying VkDevice handle
                 */
                explicit operator VkDevice() const;

                /**
                 * @brief Conversion operator to VkPhysicalDevice
                 * @return underlying VkPhysicalDevice handle
                 */
                explicit operator VkPhysicalDevice() const;

            private:
                VkDevice                             m_vkDevice;
                VkPhysicalDevice                     m_vkPhysicalDevice;
                VkPhysicalDeviceFeatures             m_vkPhysicalDeviceFeatures;
                VkPhysicalDeviceProperties           m_vkPhysicalDeviceProperties;
                VkPhysicalDeviceMemoryProperties     m_vkPhysicalDeviceMemoryProperties;

                uint32_t                             m_queueCount;

                std::vector<VkQueueFamilyProperties> m_vkQueueFamilyProperties;

                bool EnumeratePhysicalDevices(VKApplication& instance, uint32_t index);
                bool QueryPhysicalDeviceInfo();
            };
        }
    }
}
