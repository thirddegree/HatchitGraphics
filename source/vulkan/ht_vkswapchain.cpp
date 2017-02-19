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

/**
 * @file ht_vkswapchain.cpp
 * @brief VKSwapChain class implementation
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 * @author jkvargas (https://github.com/jkvargas)
 *
 * This file contains implementation for VKSwapChain class
 */

#include <ht_vkswapchain.h>
#include <ht_vkapplication.h>
#include <ht_vkdevice.h>

#include <algorithm>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKSwapChain::VKSwapChain():
                m_vkSwapChain(VK_NULL_HANDLE),
                m_vkInstance(VK_NULL_HANDLE),
                m_vkDevice(VK_NULL_HANDLE),
                m_vkPhysicalDevice(VK_NULL_HANDLE),
                m_vkSurface(VK_NULL_HANDLE),
                m_width(0), m_height(0), m_bufferCount(0)
            {

            }

            VKSwapChain::~VKSwapChain()
            {
                /**
                * Free Vulkan resource memory
                */

                if (m_vkSwapChain != VK_NULL_HANDLE)
                {
                    for (auto buffer : m_buffers)
                        vkDestroyImageView(m_vkDevice, buffer.imageView, nullptr);
                }

                if (m_vkSurface != VK_NULL_HANDLE)
                {
                    vkDestroySwapchainKHR(m_vkDevice, m_vkSwapChain, nullptr);
                    vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
                }

                m_vkSurface = VK_NULL_HANDLE;
                m_vkSwapChain = VK_NULL_HANDLE;
            }

            bool VKSwapChain::Initialize(const uint32_t width, const uint32_t height, VKApplication& instance, VKDevice& device)
            {
                m_width = width;
                m_height = height;
                m_vkInstance = static_cast<VkInstance>(instance);
                m_vkDevice = static_cast<VkDevice>(device);
                m_vkPhysicalDevice = static_cast<VkPhysicalDevice>(device);

                VkResult err = VK_SUCCESS;

                /**
                *  Initialize swapchain surface depending
                *  on platform (Win32, Linux)
                */
#ifdef HT_SYS_WINDOWS
                //Get HINSTANCE from HWND
                HWND window = static_cast<HWND>(instance.NativeWindow());
                HINSTANCE hInstance;
                hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(window, GWLP_HINSTANCE));

                VkWin32SurfaceCreateInfoKHR creationInfo = {};
                creationInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0;
                creationInfo.hinstance = hInstance;
                creationInfo.hwnd = window;

                err = vkCreateWin32SurfaceKHR(static_cast<VkInstance>(instance), &creationInfo, nullptr, &m_vkSurface);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize() [%s] Could not create VkSurface for Win32 window\n", VKErrorString(err));
                    return false;
                }
#elif defined(HT_SYS_LINUX)
                /**
                * There may also need to be a case where the underlying windowing
                * kit we are using does not link Xlib, but instead Xcb. One such
                * case I can think of is Qt, which I believe used Xcb for the Linux
                * windowing API.
                */
                VkXlibSurfaceCreateInfoKHR creationInfo;
                creationInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0;
                creationInfo.dpy = (Display*)instance.NativeDisplay();
                creationInfo.window = (Window)instance.NativeWindow();

                err = vkCreateXlibSurfaceKHR(m_vkInstance, &creationInfo, nullptr, &m_vkSurface);

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
                uint32_t queueCount = device.GetQueueCount();

                std::vector<VkBool32> supportsPresent(queueCount);
                for (uint32_t i = 0; i < queueCount; i++)
                    vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, i, m_vkSurface, &supportsPresent[i]);

                /**
                 * Search for a queue in array that supports present
                 */
                uint32_t presentQueueNodeIndex = UINT32_MAX;
                for (uint32_t i = 0; i < queueCount; i++)
                {
                    if (supportsPresent[i] == VK_TRUE)
                    {
                        presentQueueNodeIndex = i;
                        break;
                    }
                }

                /**
                 * Exit if there is not a present queue available
                 */
                if (presentQueueNodeIndex == UINT32_MAX)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Could not find a graphics and/or presenting queue!\n");
                    return false;
                }

                uint32_t formatCount = 0;
                vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, m_vkSurface, &formatCount, nullptr);
                std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, m_vkSurface, &formatCount, surfaceFormats.data());

                uint32_t presentCount = 0;
                vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, m_vkSurface, &presentCount, nullptr);
                std::vector<VkPresentModeKHR> presentModes(presentCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, m_vkSurface, &presentCount, presentModes.data());

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

                if ( !choosenFormatFound )
                    choosenSurfaceFormat = surfaceFormats[0];

                /*
                * We need to discuss the policy to choose the present modes yet
                */
                VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
                bool presentModeFound = false;

                for ( std::vector<VkPresentModeKHR>::iterator prsnt = presentModes.begin(); prsnt != presentModes.end(); ++prsnt)
                {
                    if ( *prsnt == VK_PRESENT_MODE_MAILBOX_KHR )
                    {
                        presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
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
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, m_vkSurface, &surfaceCapabilities);

                uint32_t imageCount = surfaceCapabilities.minImageCount + 1 <= surfaceCapabilities.maxImageCount ?
                    surfaceCapabilities.minImageCount + 1 : surfaceCapabilities.maxImageCount;

                VkExtent2D extent;
                if ( surfaceCapabilities.currentExtent.width != UINT32_MAX )
                    extent = surfaceCapabilities.currentExtent;
                else
                {
                    extent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, m_width));
                    extent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, m_height));
                }

                VkSwapchainCreateInfoKHR createSwapChainInfo = {};
                createSwapChainInfo.pNext = nullptr;
                createSwapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                createSwapChainInfo.surface = m_vkSurface;
                createSwapChainInfo.minImageCount = imageCount;
                createSwapChainInfo.imageColorSpace = choosenSurfaceFormat.colorSpace;
                /* I do believe in the first version we are not goig to enable occulus, vive and others... */
                createSwapChainInfo.imageArrayLayers = 1;
                createSwapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                createSwapChainInfo.preTransform = surfaceCapabilities.currentTransform;
                createSwapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                createSwapChainInfo.presentMode = presentMode;
                createSwapChainInfo.clipped = VK_TRUE;
                createSwapChainInfo.oldSwapchain = VK_NULL_HANDLE; //We will need to handle creating from previous
                createSwapChainInfo.imageExtent = extent;
                createSwapChainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
                createSwapChainInfo.flags = 0;
                createSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createSwapChainInfo.queueFamilyIndexCount = 0;
                createSwapChainInfo.pQueueFamilyIndices = nullptr;

                if ( vkCreateSwapchainKHR(m_vkDevice, &createSwapChainInfo, nullptr, &m_vkSwapChain) != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Failed to create swapchain.\n");
                    return false;
                }

                /**
                * Now we will create the image view for the swapchain. This entain grabbing
                * the swapchain image count, query for the image info and creating the ImageView create
                * structure.
                */
                m_bufferCount = 0;
                err = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &m_bufferCount, nullptr);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Failed to query swapchain image count. %s\n", VKErrorString(err));
                    return false;
                }

                std::vector<VkImage> images(m_bufferCount);
                err = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &m_bufferCount, images.data());
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSwapChain::Initialize(): Failed to query swapchain images. %s\n", VKErrorString(err));
                    return false;
                }

                m_buffers.resize(m_bufferCount);
                for (uint32_t i = 0; i < m_bufferCount; i++)
                {
                    VkImageViewCreateInfo colorAttachmentView = {};
                    colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    colorAttachmentView.pNext = NULL;
                    colorAttachmentView.format = VK_FORMAT_B8G8R8A8_UNORM;
                    colorAttachmentView.components = {
                        VK_COMPONENT_SWIZZLE_R,
                        VK_COMPONENT_SWIZZLE_G,
                        VK_COMPONENT_SWIZZLE_B,
                        VK_COMPONENT_SWIZZLE_A
                    };
                    colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    colorAttachmentView.subresourceRange.baseMipLevel = 0;
                    colorAttachmentView.subresourceRange.levelCount = 1;
                    colorAttachmentView.subresourceRange.baseArrayLayer = 0;
                    colorAttachmentView.subresourceRange.layerCount = 1;
                    colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    colorAttachmentView.image = images[i];
                    colorAttachmentView.flags = 0;

                    m_buffers[i].image = images[i];

                    err = vkCreateImageView(m_vkDevice, &colorAttachmentView, nullptr, &m_buffers[i].imageView);
                    if (err != VK_SUCCESS)
                    {
                        HT_ERROR_PRINTF("VKSwapChain::Initialize(): Failed to create image view. %s\n", VKErrorString(err));
                        return false;
                    }
                }


                return true;
            }

            bool VKSwapChain::AcquireNextImage(VkSemaphore semaphore, uint32_t *index) const
            {
                VkResult err = VK_SUCCESS;

                err = vkAcquireNextImageKHR(m_vkDevice, m_vkSwapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, index);
                if(err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKSwapChain::AcquireNextImage: Failed to acquire image index. %s\n",
                        VKErrorString(err));
                    return false;
                }

                return true;
            }

            bool VKSwapChain::QueuePresent(VkQueue queue, uint32_t index, VkSemaphore semaphore) const
            {
                VkResult err = VK_SUCCESS;

                VkPresentInfoKHR info = {};
                info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                info.pNext = nullptr;
                info.swapchainCount = 1;
                info.pSwapchains = &m_vkSwapChain;
                info.pImageIndices = &index;
                /**
                 * Check if wait semaphore has been provided
                 * to wait for before presenting image
                 */
                 if(semaphore != VK_NULL_HANDLE)
                 {
                     info.pWaitSemaphores = &semaphore;
                     info.waitSemaphoreCount = 1;
                 }

                err = vkQueuePresentKHR(queue, &info);
                if(err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKSwapChain::QueuePresent() Failed to queue swapchain for present.\n");
                    return false;
                }

                return true;
            }

            uint32_t VKSwapChain::GetBufferCount() const
            {
                return m_bufferCount;
            }

            std::vector<VKSwapChain::Buffer>& VKSwapChain::GetBuffers()
            {
                return m_buffers;
            }

            bool VKSwapChain::IsValid() const
            {
                return m_vkSwapChain != VK_NULL_HANDLE;
            }

        }
    }
}
