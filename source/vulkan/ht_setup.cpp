
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
 * \file ht_vksetup.h
 * \brief Setup class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#include <ht_vksetup.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            Setup::Setup()
            {
            }

            Setup::~Setup()
            {
            }

            void Setup::Start()
            {
                m_Application.Initialize(nullptr, nullptr);
                m_Device.Initialize(m_Application, 0);
                m_Swapchain.Initialize(m_ScreenWidth, m_ScreenHeight, m_Application, m_Device );
                m_CommandPool.Initialize(m_Device, m_Swapchain.QueueFamilyIndex());

                m_CommandPool.AllocateCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &m_CommandBuffer);

                VkCommandBufferBeginInfo pCreateInfoCmdBuffer = {};
                pCreateInfoCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                m_CommandBuffer.Begin(&pCreateInfoCmdBuffer);

                VkFormat pDepthFormat = {};
                m_Device.GetSupportedDepthFormat(pDepthFormat);

                VkImageCreateInfo pCreateInfoImage = {};
                pCreateInfoImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                pCreateInfoImage.pNext = nullptr;
                pCreateInfoImage.imageType = VK_IMAGE_TYPE_2D;
                pCreateInfoImage.format = pDepthFormat;
                pCreateInfoImage.extent = { m_ScreenWidth, m_ScreenHeight, 1};
                pCreateInfoImage.mipLevels = 1;
                pCreateInfoImage.arrayLayers = 1;
                pCreateInfoImage.tiling = VK_IMAGE_TILING_OPTIMAL;
                pCreateInfoImage.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                pCreateInfoImage.flags = 0;

                VkMemoryAllocateInfo pCreateInfoMemory = {};
                pCreateInfoMemory.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                pCreateInfoMemory.pNext = NULL;
                pCreateInfoMemory.allocationSize = 0;
                pCreateInfoMemory.memoryTypeIndex = 0;

                VkImageViewCreateInfo pDepthView = {};
                pDepthView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                pDepthView.pNext = NULL;
                pDepthView.viewType = VK_IMAGE_VIEW_TYPE_2D;
                pDepthView.format = pDepthFormat;
                pDepthView.flags = 0;
                pDepthView.subresourceRange = {};
                pDepthView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                pDepthView.subresourceRange.baseMipLevel = 0;
                pDepthView.subresourceRange.levelCount = 1;
                pDepthView.subresourceRange.baseArrayLayer = 0;
                pDepthView.subresourceRange.layerCount = 1;

                VkMemoryRequirements memReqs;

                m_ImageDepth.Initialize(m_Device, &pCreateInfoImage, &pDepthView);
                /* TODO: Maybe change to get from the correct place */
                m_ImageDepth.AllocateAndBindMemory(m_Device);


                /* Preparing the renderpass, this also, changes drastically depending on the aproach, maybe we should talk on the design */

                std::vector<VkAttachmentDescription> pAttachments {2};
                /* TODO: Get the format from the correct place */
                pAttachments[0].format = VK_FORMAT_B8G8R8A8_UNORM;
                pAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
                pAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                pAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                pAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                pAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                pAttachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                pAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                pAttachments[1].format = pDepthFormat;
                pAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
                pAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                pAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                pAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                pAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                pAttachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                pAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            }
        }
    }
}
