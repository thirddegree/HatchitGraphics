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

/**
* \class VKTools
* \ingroup HatchitGraphics
*
* \brief A series of functions needed all over the graphics library for Vulkan
*
* This class is a series of static functions that are needed in a variety of places.
* It also keeps track of a "Setup" command buffer used for setting the image layout
* of images during initialization and retrieving memory properties.
*/

#pragma once

#include <ht_platform.h>    //HT_API
#include <ht_vulkan.h>      //General Vulkan headers
#include <ht_vkdevice.h>    //Vulkan Device
#include <ht_vkqueue.h>     //Vulkan Queue

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKTools 
            {
            public:
                static bool Initialize(const VKDevice* device, const VKQueue* queue);
                static void DeInitialize();
                
                static bool CreateUniformBuffer(size_t dataSize, void* data, UniformBlock_vk* uniformBlock);
                static bool CreateTexelBuffer(size_t dataSize, void* data, TexelBlock_vk* texelBlock);

                static void DeleteUniformBuffer(UniformBlock_vk& uniformBlock);
                static void DeleteTexelBuffer(TexelBlock_vk& texelBlock);

                static VkFormat GetPreferredColorFormat();
                static VkFormat GetPreferredDepthFormat();

                static void CreateSetupCommandBuffer();
                static void FlushSetupCommandBuffer();
                static VkCommandBuffer GetSetupCommandBuffer();

                //Reused helpers
                static bool SetImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
                static bool SetImageLayout(VkImage image, VkImageAspectFlags aspectMask,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
                static bool MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex);

            private:
                static VkCommandPool                    m_setupCommandPool;
                static VkCommandBuffer                  m_setupCommandBuffer;
                static VkDevice                         m_device;
                static VkQueue                          m_queue;
                static VkPhysicalDeviceMemoryProperties m_gpuMemoryProps;

            };

        }
    }
}
