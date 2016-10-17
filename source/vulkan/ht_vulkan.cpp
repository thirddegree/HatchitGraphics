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

#include <ht_vulkan.h>
#include <ht_debug.h>

namespace Hatchit 
{
    namespace Graphics 
    {
        namespace Vulkan 
        {
            
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

            std::string VKErrorString(VkResult code)
            {
#define VK_RES_CASE(x) case VK_ ##x: return HT_SFY_(x)
                switch (code)
                {
                    VK_RES_CASE(NOT_READY);
                    VK_RES_CASE(TIMEOUT);
                    VK_RES_CASE(EVENT_SET);
                    VK_RES_CASE(EVENT_RESET);
                    VK_RES_CASE(INCOMPLETE);
                    VK_RES_CASE(ERROR_OUT_OF_HOST_MEMORY);
                    VK_RES_CASE(ERROR_OUT_OF_DEVICE_MEMORY);
                    VK_RES_CASE(ERROR_INITIALIZATION_FAILED);
                    VK_RES_CASE(ERROR_DEVICE_LOST);
                    VK_RES_CASE(ERROR_MEMORY_MAP_FAILED);
                    VK_RES_CASE(ERROR_LAYER_NOT_PRESENT);
                    VK_RES_CASE(ERROR_EXTENSION_NOT_PRESENT);
                    VK_RES_CASE(ERROR_FEATURE_NOT_PRESENT);
                    VK_RES_CASE(ERROR_INCOMPATIBLE_DRIVER);
                    VK_RES_CASE(ERROR_TOO_MANY_OBJECTS);
                    VK_RES_CASE(ERROR_FORMAT_NOT_SUPPORTED);
                    VK_RES_CASE(ERROR_SURFACE_LOST_KHR);
                    VK_RES_CASE(ERROR_NATIVE_WINDOW_IN_USE_KHR);
                    VK_RES_CASE(SUBOPTIMAL_KHR);
                    VK_RES_CASE(ERROR_OUT_OF_DATE_KHR);
                    VK_RES_CASE(ERROR_INCOMPATIBLE_DISPLAY_KHR);
                    VK_RES_CASE(ERROR_VALIDATION_FAILED_EXT);
                    VK_RES_CASE(ERROR_INVALID_SHADER_NV);
                default:
                    return "UNKNOWN_ERROR";
                }
#undef VK_RES_CASE
            }

        }
    }
}