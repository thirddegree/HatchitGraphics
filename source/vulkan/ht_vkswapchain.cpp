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

#include <ht_vkswapchain.h>
#include <ht_vkapplication.h>
#include <ht_vkdevice.h>
#include <ht_debug.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKSwapChain::VKSwapChain()
            {
                m_surface = VK_NULL_HANDLE;
            }

            VKSwapChain::~VKSwapChain()
            {

            }

            bool VKSwapChain::Initialize(VKApplication& instance, VKDevice& device)
            {
                VkResult err = VK_SUCCESS;

                /*
                *  Initialize swapchain surface depending
                *  on platform (Win32, Linux)
                */
#ifdef HT_SYS_WINDOWS
                //Get HINSTANCE from HWND
                HWND window = (HWND)instance.NativeWindow();
                HINSTANCE hInstance;
                hInstance = (HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE);

                VkWin32SurfaceCreateInfoKHR creationInfo = {};
                creationInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0;
                creationInfo.hinstance = hInstance;
                creationInfo.hwnd = window;

                err = vkCreateWin32SurfaceKHR(instance, &creationInfo, nullptr, &m_surface);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize() [%s] Could not create VkSurface for Win32 window\n", VKErrorString(err));
                    return false;
                }
#elif defined(HT_SYS_LINUX)
                VkXlibSurfaceCreateInfoKHR creationInfo;
                creationInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0;
                creationInfo.dpy = (Display*)instance.NativeDisplay();
                creationInfo.window = (Window)instance.NativeWindow();

                err = vkCreateXlibSurfaceKHR(instance, &creationInfo, nullptr, &m_surface);

                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): [%s] Could not create VkSurface for XLib window\n", VKErrorString(err));
                    return false;
                }
#endif

                /**
                * At this point we should now have a valid
                * vulkan swapchain surface handle. Next we enumerate
                * the physical device queue properties. This is to make sure
                * we have a valid logical device to present with
                */
                uint32_t queueCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
                if (queueCount < 1)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Invalid queue count.\n");
                    return false;
                }

                std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueProperties.data());
                
                std::vector<VkBool32> supportsPresent(queueCount);
                for (uint32_t i = 0; i < queueCount; i++)
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &supportsPresent[i]);

                // Search for a graphics and a present queue in the array of queue
                // families, try to find one that supports both
                uint32_t graphicsQueueNodeIndex = UINT32_MAX;
                uint32_t presentQueueNodeIndex = UINT32_MAX;
                for (uint32_t i = 0; i < queueCount; i++)
                {
                    if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                    {
                        if (graphicsQueueNodeIndex == UINT32_MAX)
                        {
                            graphicsQueueNodeIndex = i;
                        }

                        if (supportsPresent[i] == VK_TRUE)
                        {
                            graphicsQueueNodeIndex = i;
                            presentQueueNodeIndex = i;
                            break;
                        }
                    }
                }
                if (presentQueueNodeIndex == UINT32_MAX)
                {
                    // If there's no queue that supports both present and graphics
                    // try to find a separate present queue
                    for (uint32_t i = 0; i < queueCount; ++i)
                    {
                        if (supportsPresent[i] == VK_TRUE)
                        {
                            presentQueueNodeIndex = i;
                            break;
                        }
                    }
                }

                /**
                *   Exit if there is neither a graphics nor a present queue available
                */
                if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Could not find a graphics and/or presenting queue!\n");
                    return false;
                }

                /**
                *   TODO:
                *       Add support for separate graphics and presenting queue
                */
                if (graphicsQueueNodeIndex != presentQueueNodeIndex)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Separate graphics and presenting queues are not supported yet!\n");
                    return false;
                }

                return true;
            }

            bool VKSwapChain::IsValid()
            {
                /*
                * Check to make sure swapchain 
                * was initialized properly
                */

                return true;
            }
        }
    }
}
