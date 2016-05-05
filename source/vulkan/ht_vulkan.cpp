/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
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

#include <ht_vulkan.h>
#include <cassert>
#include <ht_debug.h>

namespace Hatchit 
{
    namespace Graphics 
    {
        namespace Vulkan 
        {
            PFN_vkGetPhysicalDeviceSurfaceSupportKHR
                fpGetPhysicalDeviceSurfaceSupportKHR;
            PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
                fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
            PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
                fpGetPhysicalDeviceSurfaceFormatsKHR;
            PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
                fpGetPhysicalDeviceSurfacePresentModesKHR;

            PFN_vkCreateDebugReportCallbackEXT
                fpCreateDebugReportCallback;
            PFN_vkDestroyDebugReportCallbackEXT
                fpDestroyDebugReportCallback;
            PFN_vkDebugReportMessageEXT
                fpDebugReportMessage;

            PFN_vkCreateSwapchainKHR
                fpCreateSwapchainKHR;
            PFN_vkDestroySwapchainKHR
                fpDestroySwapchainKHR;
            PFN_vkGetSwapchainImagesKHR
                fpGetSwapchainImagesKHR;
            PFN_vkAcquireNextImageKHR
                fpAcquireNextImageKHR;
            PFN_vkQueuePresentKHR
                fpQueuePresentKHR;
        }
    }
}
