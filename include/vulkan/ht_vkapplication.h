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
#include <ht_vulkan.h>
#include <ht_string.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;

            /**
             * \class VKApplication
             * \brief Defines a Vulkan application instance
             *
             * A Vulkan application instance stores the per-application vulkan states
             */
            class HT_API VKApplication
            {
            public:
                VKApplication();
                VKApplication(const VkApplicationInfo& info);

                ~VKApplication();

                bool Initialize();
                bool IsValid();

                const std::string   Name()              const;
                const uint32_t      Version()           const;
                const std::string&  EngineName()        const;
                const uint32_t      EngineVersion()     const;
                const uint32_t      APIVersion()        const;

                VKDevice* const     Device(uint32_t index);

            private:
                VkInstance                          m_instance;
                VkApplicationInfo                   m_info;

                std::vector<std::string>            m_layers;
                std::vector<std::string>            m_extensions;

                std::vector<VkPhysicalDevice>       m_devices;

            private:
                bool CheckInstanceLayers();
                bool CheckInstanceExtensions();
                bool EnumeratePhysicalDevices();


                static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                    uint64_t srcObject, size_t location, int32_t msgCode,
                    const char *pLayerPrefix, const char *pMsg, void *pUserData);
            };
        }
    }
}