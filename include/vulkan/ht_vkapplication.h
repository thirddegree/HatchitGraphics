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
 * \file ht_vkapplication.h
 * \brief VKApplication class definition
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKApplication class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>
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
             * \brief Defines a Vulkan application instance.
             * 
             *  A Vulkan application instance stores the per-application vulkan states.
             */
            class HT_API VKApplication
            {
            public:
                VKApplication();
                explicit VKApplication(const VkApplicationInfo& info);

                ~VKApplication();

                bool Initialize(void* window, void* display);
                bool IsValid() const;

                std::string   Name()              const;
                uint32_t      Version()           const;
                std::string   EngineName()        const;
                uint32_t      EngineVersion()     const;
                uint32_t      APIVersion()        const;

                uint32_t      EnabledLayerCount() const;
                uint32_t      EnabledExtensionCount() const;

                const std::vector<std::string>& EnabledLayerNames() const;
                const std::vector<std::string>& EnabledExtensionNames() const;

                void* NativeWindow() const;
                void* NativeDisplay() const;

                explicit operator VkInstance() const;

            private:
                VkInstance                          m_instance;
                VkApplicationInfo                   m_info;
                VkSurfaceKHR                        m_window;
                void*                               m_nativeWindow;
                void*                               m_nativeDisplay;
                std::vector<std::string>            m_layers;
                std::vector<std::string>            m_extensions;

                VkDebugReportCallbackEXT            m_debugReportCallback;

            private:
                bool CheckInstanceLayers();
                bool CheckInstanceExtensions();

                static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                    uint64_t srcObject, size_t location, int32_t msgCode,
                    const char *pLayerPrefix, const char *pMsg, void *pUserData);
            };
        }
    }
}