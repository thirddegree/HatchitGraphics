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
#include <ht_device.h>
#include <ht_vulkan.h>
#include <ht_debug.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKDevice : public IDevice
            {
            public:
                VKDevice();

                ~VKDevice();

                bool VInitialize()          override;
                void VReportDeviceInfo()    override;

                void SetValidation(bool validate);

                const std::vector<VkDevice>&                            GetVKDevices() const;
                const std::vector<VkPhysicalDevice>&                    GetVKPhysicalDevices() const;
                const std::vector<VkPhysicalDeviceFeatures>&            GetVKPhysicalDeviceFeatures() const;
                const std::vector<VkPhysicalDeviceMemoryProperties>&    GetVKPhysicalDeviceMemoryProperties() const;
                const VkInstance&                                       GetVKInstance() const;

            private:
                std::vector<VkDevice>                           m_devices;
                std::vector<VkPhysicalDevice>                   m_gpus;
                std::vector<VkPhysicalDeviceFeatures>           m_gpuFeatures;
                std::vector<VkPhysicalDeviceMemoryProperties>   m_gpuMemoryProps;
                VkInstance                                      m_instance;

                bool    m_initialized;
                bool    m_validate;

                VkDebugReportCallbackEXT m_debugReportCallback;

                //The various known layer names for different SDK versions
                std::vector<const char*> m_layerNames1013;
                std::vector<const char*> m_layerNames1011;
                std::vector<const char*> m_layerNames108;
                std::vector<const char*> m_layerNames105;
                std::vector<const char*> m_layerNames103;

                //Collection of possible name collections
                std::vector<std::vector<const char*>> m_layerNamesCollection;

                //The actual extension and layer names to check 
                std::vector<const char*> m_enabledExtensionNames;
                std::vector<const char*> m_enabledLayerNames;

                bool setupInstance();
                bool enumeratePhysicalDevices();
                bool queryDeviceCapabilities();
                bool setupDevices();
                bool setupProcAddresses();
                bool setupDebugCallback();

                bool checkInstanceLayers();
                bool checkInstanceExtensions();
                bool checkDeviceLayers(const VkPhysicalDevice& gpu);
                bool checkDeviceExtensions(const VkPhysicalDevice& gpu);

                bool checkLayers(std::vector<const char*> layerNames, std::vector <VkLayerProperties> layers);

                static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                    uint64_t srcObject, size_t location, int32_t msgCode,
                    const char *pLayerPrefix, const char *pMsg, void *pUserData);
            };
        }
    }
}
