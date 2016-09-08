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
           
            extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR
                fpGetPhysicalDeviceSurfaceSupportKHR;
            extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
                fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
            extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
                fpGetPhysicalDeviceSurfaceFormatsKHR;
            extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
                fpGetPhysicalDeviceSurfacePresentModesKHR;

            extern PFN_vkCreateDebugReportCallbackEXT
                fpCreateDebugReportCallback;
            extern PFN_vkDestroyDebugReportCallbackEXT
                fpDestroyDebugReportCallback;
            extern PFN_vkDebugReportMessageEXT
                fpDebugReportMessage;

            extern PFN_vkCreateSwapchainKHR
                fpCreateSwapchainKHR;
            extern  PFN_vkDestroySwapchainKHR
                fpDestroySwapchainKHR;
            extern PFN_vkGetSwapchainImagesKHR
                fpGetSwapchainImagesKHR;
            extern PFN_vkAcquireNextImageKHR
                fpAcquireNextImageKHR;
            extern PFN_vkQueuePresentKHR
                fpQueuePresentKHR;
        }
    }
}
