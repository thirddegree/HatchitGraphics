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
 * @file ht_vkrenderpass.h
 * @brief VKRenderPass class definition
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 * 
 * This file contains definition for VKRenderPass class
 */

#pragma once

#include <ht_platform.h>
#include <ht_vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;
            class VKSwapChain;

            /**
             * @class VKRenderPass
             * @brief Defines a default Vulkan renderpass
             * 
             * Default renderpass that shoud be compatible
             * with most swapchain setups. 
             * 
             * @note
             *      There are plans to expand this to be created
             *      from a resource so that compatible renderpass and
             *      pipeline objects can be user defined
             */
            class HT_API VKRenderPass
            {
            public:
                /**
                 * @brief Constructs renderpass
                 */
                VKRenderPass();

                /**
                 * @brief Destructs renderpass
                 * 
                 * Destroys underlying Vulkan rendpass object
                 */
                ~VKRenderPass();

                /**
                 * @brief Initializes renderpass
                 * 
                 * Initializes Vulkan renderpass matching the surface
                 * format properties of the swapchain
                 * 
                 * @param device Vulkan device
                 * @param swapchain Vulkan swapchain
                 * @return [true] : success
                 * @return [false] : failure
                 */
                bool Initialize(VKDevice& device, VKSwapChain& swapchain);

                /**
                 * @brief Initializes renderpass
                 * 
                 * Initializes Vulkan renderpass from provided
                 * renderpass creation info
                 * 
                 * @param device Vulkan device
                 * @param info Vulkan renderpass creation info
                 * @return [true] : success
                 * @return [false] : failure
                 */
                bool Initialize(VKDevice& device, const VkRenderPassCreateInfo& info);

                /**
                 * @breif Conversion to VkRenderPass handle
                 * 
                 * @return underlying VkRenderPass handle
                 */
                explicit operator VkRenderPass() const;

                /**
                 * @breif Conversion to pointer to VkRenderPass handle
                 * 
                 * @return pointer to underlying VkRenderPass handle
                 */
                explicit operator VkRenderPass*();

            protected:
                VkRenderPass m_vkRenderPass;
                VkDevice     m_vkDevice;
            };
        }
    }
}
