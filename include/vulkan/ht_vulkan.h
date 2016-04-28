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

#pragma once

#include <ht_platform.h>

#ifdef HT_SYS_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#ifdef HT_SYS_LINUX
#define VK_USE_PLATFORM_XCB_KHR 1
#endif

#include <vulkan/vulkan.h>

namespace Hatchit {
    namespace Graphics {
        namespace Vulkan {

            struct LayoutLocation {
                uint32_t set;
                uint32_t binding;
            };

            struct UniformBlock_vk
            {
                VkBuffer                buffer;
                VkDeviceMemory          memory;
                VkDescriptorBufferInfo  descriptor;
            };

            struct TexelBlock_vk
            {
                VkBuffer                buffer;
                VkDeviceMemory          memory;
                VkBufferView            view;
            };

            struct Image_vk
            {
                VkImage         image;
                VkImageView     view;
                VkDeviceMemory  memory;
            };

            struct Texture_vk
            {
                VkSampler sampler;
                VkImageLayout layout;
                Image_vk image;
                uint32_t width, height;
                uint32_t mipLevels;
            };

            bool CreateUniformBuffer(const VkDevice& device, size_t dataSize, void* data, UniformBlock_vk* uniformBlock);
            bool CreateTexelBuffer(const VkDevice& device, size_t dataSize, void* data, TexelBlock_vk* texelBlock);

            void DeleteUniformBuffer(const VkDevice& device, UniformBlock_vk& uniformBlock);
            void DeleteTexelBuffer(const VkDevice& device, TexelBlock_vk& texelBlock);

            extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR
                fpGetPhysicalDeviceSurfaceSupportKHR;
            extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
                fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
            extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
                fpGetPhysicalDeviceSurfaceFormatsKHR;
            extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
                fpGetPhysicalDeviceSurfacePresentModesKHR;

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
