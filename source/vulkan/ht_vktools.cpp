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

#include <ht_vktools.h>

namespace Hatchit 
{
    namespace Graphics 
    {
        namespace Vulkan
        {
            VkCommandPool                    VKTools::m_setupCommandPool;
            VkCommandBuffer                  VKTools::m_setupCommandBuffer;
            VkDevice                         VKTools::m_device;
            VkQueue                          VKTools::m_queue;
            VkPhysicalDeviceMemoryProperties VKTools::m_gpuMemoryProps;

            bool VKTools::Initialize(const VKDevice* device, const VKQueue* queue) 
            {
                m_device = device->GetVKDevices()[0];
                m_gpuMemoryProps = device->GetVKPhysicalDeviceMemoryProperties()[0];

                if (queue->GetQueueType() != QueueType::GRAPHICS)
                {
                    HT_ERROR_PRINTF("VKTools::Initialize: Must be given a valid graphics queue");
                    return false;
                }

                //Got a valid queue, lets just get the VkQueue inside it
                m_queue = queue->GetVKQueue();

                m_setupCommandBuffer = VK_NULL_HANDLE;

                VkResult err;

                //Create the command pool for this buffer
                VkCommandPoolCreateInfo commandPoolInfo = {};
                commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                commandPoolInfo.pNext = nullptr;
                commandPoolInfo.flags = 0;
                commandPoolInfo.queueFamilyIndex = 0;

                err = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_setupCommandPool);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKTools::Initialize: Could not create a command pool");
                    return false;
                }

                return true;
            }
            void VKTools::DeInitialize() 
            {
                //Reset and destroy all memory related to setup command pool and buffer
                vkResetCommandPool(m_device, m_setupCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

                vkFreeCommandBuffers(m_device, m_setupCommandPool, 1, &m_setupCommandBuffer);

                vkDestroyCommandPool(m_device, m_setupCommandPool, nullptr);
            }

            bool VKTools::CreateUniformBuffer(size_t dataSize, void* data, UniformBlock_vk* uniformBlock) 
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &uniformBlock->buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to create buffer\n");
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(m_device, uniformBlock->buffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to get memory type\n");
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &uniformBlock->memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to allocate memory\n");
                    return false;
                }

                //We may not ask for a buffer that has anything in it
                if (data != nullptr)
                {
                    err = vkMapMemory(m_device, uniformBlock->memory, 0, dataSize, 0, &pData);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to map memory\n");
                        return false;
                    }

                    //Actually copy data into location
                    memcpy(pData, data, dataSize);

                    //Unmap memory and then bind to the uniform
                    vkUnmapMemory(m_device, uniformBlock->memory);
                }

                err = vkBindBufferMemory(m_device, uniformBlock->buffer, uniformBlock->memory, 0);
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
            bool VKTools::CreateTexelBuffer(size_t dataSize, void* data, TexelBlock_vk* texelBlock) 
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &texelBlock->buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to create buffer\n");
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(m_device, texelBlock->buffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to get memory type\n");
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &texelBlock->memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to allocate memory\n");
                    return false;
                }

                //We may not ask for a buffer that has anything in it
                if (data != nullptr)
                {
                    err = vkMapMemory(m_device, texelBlock->memory, 0, dataSize, 0, &pData);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKMesh::createBuffer(): Failed to map memory\n");
                        return false;
                    }

                    //Actually copy data into location
                    memcpy(pData, data, dataSize);

                    //Unmap memory and then bind to the uniform
                    vkUnmapMemory(m_device, texelBlock->memory);
                }

                err = vkBindBufferMemory(m_device, texelBlock->buffer, texelBlock->memory, 0);
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

                vkCreateBufferView(m_device, &viewInfo, nullptr, &texelBlock->view);

                return true;
            }

            void VKTools::DeleteUniformBuffer(UniformBlock_vk& uniformBlock) 
            {
                vkDestroyBuffer(m_device, uniformBlock.buffer, nullptr);
                vkFreeMemory(m_device, uniformBlock.memory, nullptr);
            }
            void VKTools::DeleteTexelBuffer(TexelBlock_vk& texelBlock) 
            {
                vkDestroyBufferView(m_device, texelBlock.view, nullptr);
                vkDestroyBuffer(m_device, texelBlock.buffer, nullptr);
                vkFreeMemory(m_device, texelBlock.memory, nullptr);
            }

            VkFormat VKTools::GetPreferredColorFormat()
            {
                return VK_FORMAT_R8G8B8A8_UNORM;
            }
            VkFormat VKTools::GetPreferredDepthFormat()
            {
                return VK_FORMAT_D32_SFLOAT;
            }

            void VKTools::CreateSetupCommandBuffer() 
            {
                if (m_setupCommandBuffer != VK_NULL_HANDLE)
                {
                    vkFreeCommandBuffers(m_device, m_setupCommandPool, 1, &m_setupCommandBuffer);
                    m_setupCommandBuffer = VK_NULL_HANDLE;
                }

                VkResult err;

                //Start up a basic command buffer if we don't have one already
                if (m_setupCommandBuffer == VK_NULL_HANDLE)
                {
                    VkCommandBufferAllocateInfo command;
                    command.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    command.pNext = nullptr;
                    command.commandPool = m_setupCommandPool;
                    command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    command.commandBufferCount = 1;

                    err = vkAllocateCommandBuffers(m_device, &command, &m_setupCommandBuffer);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("VKRenderer::CreateSetupCommandBuffer(): Failed to allocate command buffer.\n");
                    }
                }

                VkCommandBufferBeginInfo cmdBufInfo = {};
                cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                err = vkBeginCommandBuffer(m_setupCommandBuffer, &cmdBufInfo);
                assert(!err);
            }

            void VKTools::FlushSetupCommandBuffer() 
            {
                VkResult err;
                if (m_setupCommandBuffer == VK_NULL_HANDLE)
                    return;

                err = vkEndCommandBuffer(m_setupCommandBuffer);
                assert(!err);

                VkFence nullFence = VK_NULL_HANDLE;

                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.waitSemaphoreCount = 0;
                submitInfo.pWaitSemaphores = nullptr;
                submitInfo.pWaitDstStageMask = nullptr;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &m_setupCommandBuffer;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;

                err = vkQueueSubmit(m_queue, 1, &submitInfo, nullFence);
                assert(!err);

                err = vkQueueWaitIdle(m_queue);
                assert(!err);

                vkFreeCommandBuffers(m_device, m_setupCommandPool, 1, &m_setupCommandBuffer);
                m_setupCommandBuffer = VK_NULL_HANDLE;
            }

            VkCommandBuffer VKTools::GetSetupCommandBuffer() { return m_setupCommandBuffer; }

            //Reused helpers
            bool VKTools::SetImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask,
                VkImageLayout oldImageLayout, VkImageLayout newImageLayout) 
            {
                VkImageMemoryBarrier imageMemoryBarrier;
                imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageMemoryBarrier.pNext = nullptr;
                imageMemoryBarrier.srcAccessMask = 0;
                imageMemoryBarrier.dstAccessMask = 0;
                imageMemoryBarrier.oldLayout = oldImageLayout;
                imageMemoryBarrier.newLayout = newImageLayout;
                imageMemoryBarrier.image = image;
                imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
                imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
                imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
                imageMemoryBarrier.subresourceRange.layerCount = 1;
                imageMemoryBarrier.subresourceRange.levelCount = 1;
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                // Undefined layout
                // Only allowed as initial layout!
                // Make sure any writes to the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // Old layout is color attachment
                // Make sure any writes to the color buffer have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                }

                // Old layout is transfer source
                // Make sure any reads from the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // Old layout is shader read (sampler, input attachment)
                // Make sure any shader reads from the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                }

                // Target layouts (new)

                // New layout is transfer destination (copy, blit)
                // Make sure any copyies to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // New layout is transfer source (copy, blit)
                // Make sure any reads from and writes to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                {
                    //imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                // New layout is color attachment
                // Make sure any writes to the color buffer hav been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                // New layout is depth attachment
                // Make sure any writes to depth/stencil buffer have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                }

                // New layout is shader read (sampler, input attachment)
                // Make sure any writes to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                {
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                }

                if (newImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                }

                // Put barrier on top
                VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

                vkCmdPipelineBarrier(commandBuffer, srcStageFlags, destStageFlags, 0, 0, nullptr, 0,
                    nullptr, 1, &imageMemoryBarrier);

                return true;
            }

            bool VKTools::SetImageLayout(VkImage image, VkImageAspectFlags aspectMask,
                VkImageLayout oldImageLayout, VkImageLayout newImageLayout) 
            {
                return SetImageLayout(m_setupCommandBuffer, image, aspectMask, oldImageLayout, newImageLayout);
            }
            
            bool VKTools::MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex) 
            {
                VkPhysicalDeviceMemoryProperties memoryProps = m_gpuMemoryProps;

                //Search mem types to find the first index with those properties
                for (uint32_t i = 0; i < 32; i++)
                {
                    if ((typeBits & 1) == 1)
                    {
                        //Type exists; does it match properties we're looking for?
                        if ((memoryProps.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
                        {
                            *typeIndex = i;
                            return true;
                        }
                    }
                    typeBits >>= 1;
                }

                return false; //nothing found
            }
        }
    }
}