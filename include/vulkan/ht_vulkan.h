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
 * \file ht_vulkan.h
 * \brief Vulkan include file
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains various Vulkan utilities. It also
 * includes the master vulkan.h from the Vulkan SDK.
 */

#include <ht_platform.h>
#include <ht_debug.h>
#include <ht_string.h>

#ifdef HT_SYS_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#ifdef HT_SYS_LINUX
#define VK_USE_PLATFORM_XLIB_KHR 1
#endif

#include <vulkan/vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            std::string VKErrorString(VkResult code);

            template<typename T>
            bool GetInstanceProcAddr(VkInstance instance, const char* entry, T** func)
            {
                *func = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, entry));
                if (*func == nullptr)
                {
                    HT_ERROR_PRINTF("Vulkan::GetInstanceProcAddr(): Failed to get %s address\n",
                        entry);
                    return false;
                }

                return true;
            }

            template<typename T>
            bool GetDeviceProcAddr(VkDevice device, const char* entry, T** func)
            {
                *func = reinterpret_cast<T*>(vkGetDeviceProcAddr(device, entry));
                if (*func == nullptr)
                {
                    HT_ERROR_PRINTF("Vulkan::GetDeviceProcAddr(): Failed to get %s address\n",
                        entry);
                    return false;
                }

                return true;
            }
        }
    }
}
