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
#include <ht_vkrenderer.h>

namespace Hatchit {
    namespace Graphics {
        namespace Vulkan {

            bool CreateUniformBuffer(const VkDevice& device, size_t dataSize, void* data, UniformBlock_vk* uniformBlock)
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &uniformBlock->buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to create buffer\n");
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(device, uniformBlock->buffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = VKRenderer::RendererInstance->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to get memory type\n");
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, &uniformBlock->memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to allocate memory\n");
                    return false;
                }

                //We may not ask for a buffer that has anything in it
                if (data != nullptr)
                {
                    err = vkMapMemory(device, uniformBlock->memory, 0, dataSize, 0, &pData);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to map memory\n");
                        return false;
                    }

                    //Actually copy data into location
                    memcpy(pData, data, dataSize);

                    //Unmap memory and then bind to the uniform
                    vkUnmapMemory(device, uniformBlock->memory);
                }

                err = vkBindBufferMemory(device, uniformBlock->buffer, uniformBlock->memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::VBuffer(): Failed to bind memory\n");
                    return false;
                }

                uniformBlock->descriptor.buffer = uniformBlock->buffer;
                uniformBlock->descriptor.offset = 0;
                uniformBlock->descriptor.range = dataSize;

                return true;
            }

            bool CreateTexelBuffer(const VkDevice& device, size_t dataSize, void* data, TexelBlock_vk* texelBlock) 
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &texelBlock->buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to create buffer\n");
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(device, texelBlock->buffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = VKRenderer::RendererInstance->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to get memory type\n");
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, &texelBlock->memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to allocate memory\n");
                    return false;
                }

                //We may not ask for a buffer that has anything in it
                if (data != nullptr)
                {
                    err = vkMapMemory(device, texelBlock->memory, 0, dataSize, 0, &pData);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to map memory\n");
                        return false;
                    }

                    //Actually copy data into location
                    memcpy(pData, data, dataSize);

                    //Unmap memory and then bind to the uniform
                    vkUnmapMemory(device, texelBlock->memory);
                }

                err = vkBindBufferMemory(device, texelBlock->buffer, texelBlock->memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::VBuffer(): Failed to bind memory\n");
                    return false;
                }

                //Create a buffer view
                VkBufferViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
                viewInfo.pNext = nullptr;
                viewInfo.flags = 0;
                viewInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
                viewInfo.offset = 0;
                viewInfo.range = dataSize;
                viewInfo.buffer = texelBlock->buffer;

                vkCreateBufferView(device, &viewInfo, nullptr, &texelBlock->view);

                return true;
            }

            void DeleteUniformBuffer(const VkDevice& device, UniformBlock_vk& uniformBlock)
            {
                vkDestroyBuffer(device, uniformBlock.buffer, nullptr);
                vkFreeMemory(device, uniformBlock.memory, nullptr);
            }

            void DeleteTexelBuffer(const VkDevice& device, TexelBlock_vk& texelBlock)
            {
                vkDestroyBufferView(device, texelBlock.view, nullptr);
                vkDestroyBuffer(device, texelBlock.buffer, nullptr);
                vkFreeMemory(device, texelBlock.memory, nullptr);
            }

            PFN_vkGetPhysicalDeviceSurfaceSupportKHR
                fpGetPhysicalDeviceSurfaceSupportKHR;
            PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
                fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
            PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
                fpGetPhysicalDeviceSurfaceFormatsKHR;
            PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
                fpGetPhysicalDeviceSurfacePresentModesKHR;

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
