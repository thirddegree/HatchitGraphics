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
                m_swapchain = VK_NULL_HANDLE;
            }

            VKSwapChain::~VKSwapChain()
            {
                /* How you want to hold the reference for the instance?

                vkDestroySurfaceKHR(instanceReference, m_surface, nullptr);
                vkDestroySwapchainKHR(instancereference, m_swapchain, nullptr);
                */
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

                /* if (graphicsQueueNodeIndex != presentQueueNodeIndex)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): different queues not yet supported.\n");
                    return false;
                } */

                uint32_t formatCount = 0;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
                std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, surfaceFormats.data());

                uint32_t presentCount = 0;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentCount, nullptr);
                std::vector<VkPresentModeKHR> presentModes(presentCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentCount, presentModes.data());

                if ( surfaceFormats.empty() || presentModes.empty() )
                {
                    HT_ERROR_PRINTF("VkSwapChain::Initialize(): Swap chain is not supported.\n");
                    return false;
                }

                /*
                * We need to discuss how the policy to choose the formats.
                * Try to find a specific surface format, otherwise gets the first result
                */
                VkSurfaceFormatKHR choosenSurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
                bool choosenFormatFound = false;

                for ( std::vector<VkSurfaceFormatKHR>::iterator sfor = surfaceFormats.begin(); sfor != surfaceFormats.end(); ++sfor)
                {
                    if (( sfor->format == VK_FORMAT_UNDEFINED ) || ( sfor->format == choosenSurfaceFormat.format && sfor->colorSpace == choosenSurfaceFormat.colorSpace ))
                    {
                        choosenFormatFound = true;
                        break;
                    }
                }

                choosenSurfaceFormat = surfaceFormats[0];

                /*
                * We need to discuss the policy to choose the present modes yet
                */
                VkPresentModeKHR choosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
                bool presentModeFound = false;

                for ( std::vector<VkPresentModeKHR>::iterator prsnt = presentModes.begin(); prsnt != presentModes.end(); ++prsnt)
                {
                    if ( *prsnt == VK_PRESENT_MODE_MAILBOX_KHR )
                    {
                        choosenPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        presentModeFound = true;
                        break;
                    }

                    if ( *prsnt == VK_PRESENT_MODE_FIFO_KHR )
                        presentModeFound = true;
                }

                if ( !presentModeFound )
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Default present mode not found.\n");
                    return false;
                }

                VkSurfaceCapabilitiesKHR surfaceCapabilities;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &surfaceCapabilities); 

                uint32_t imageCount = surfaceCapabilities.minImageCount + 1 <= surfaceCapabilities.maxImageCount ?
                    surfaceCapabilities.minImageCount + 1 : surfaceCapabilities.maxImageCount;

                VkExtent2D choosenExtent;

                if ( surfaceCapabilities.currentExtent.width != UINT32_MAX )
                    choosenExtent = surfaceCapabilities.currentExtent;
                else
                {
                    choosenExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, m_width));
                    choosenExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, m_height));
                }

                VkSwapchainCreateInfoKHR createSwapChainInfo;
                createSwapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                createSwapChainInfo.surface = m_surface;
                createSwapChainInfo.minImageCount = imageCount;
                createSwapChainInfo.imageColorSpace = choosenSurfaceFormat.colorSpace;
                /* I do believe in the first version we are not goig to enable occulus, vive and others... */
                createSwapChainInfo.imageArrayLayers = 1;
                createSwapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                createSwapChainInfo.preTransform = surfaceCapabilities.currentTransform;
                createSwapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                createSwapChainInfo.presentMode = choosenPresentMode;
                createSwapChainInfo.clipped = VK_TRUE;
                createSwapChainInfo.oldSwapchain = VK_NULL_HANDLE;
                createSwapChainInfo.imageExtent = choosenExtent;

                if ( graphicsQueueNodeIndex != presentQueueNodeIndex )
                {
                    createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                    createSwapChainInfo.queueFamilyIndexCount = 2;
                    uint32_t familiesIndices[2] = {graphicsQueueNodeIndex, presentQueueNodeIndex};
                    createSwapChainInfo.pQueueFamilyIndices = familiesIndices;
                }
                else
                    createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

                if ( vkCreateSwapchainKHR(device, &createSwapChainInfo, nullptr, &m_swapchain) != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Failed to create swapchain.\n");
                    return false;
                }

                return true;
            }

            bool VKSwapChain::IsValid()
            {
                return m_swapchain != VK_NULL_HANDLE; 
            }
        }
    }
}
