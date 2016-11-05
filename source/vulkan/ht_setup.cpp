
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

            void Setup::Initialize()
            {
                m_Application.Initialize(nullptr, nullptr);
                m_Device.Initialize(m_Application, 0);
                m_Swapchain.Initialize(m_ScreenWidth, m_ScreenHeight, m_Application, m_Device );
                m_CommandPool.Initialize(m_Device, m_Swapchain.QueueFamilyIndex());

                m_CommandPool.AllocateCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &m_CommandBuffer);

                VkCommandBufferBeginInfo pCreateInfoCmdBuffer = {};
                pCreateInfoCmdBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                m_CommandBuffer.Begin(&pCreateInfoCmdBuffer);

                VkFormat pDepthFormat{};
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

                VkAttachmentReference colorReference{};
                colorReference.attachment = 0;
                colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthReference{};
                depthReference.attachment = 0;
                depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpassDescription = {};
                subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpassDescription.colorAttachmentCount = 1;
                subpassDescription.pColorAttachments = &colorReference;
                subpassDescription.pDepthStencilAttachment = &depthReference;
                subpassDescription.inputAttachmentCount = 0;
                subpassDescription.pInputAttachments = nullptr;
                subpassDescription.preserveAttachmentCount = 0;
                subpassDescription.pPreserveAttachments = nullptr;
                subpassDescription.pResolveAttachments = nullptr;

                std::vector<VkSubpassDependency> dependencies(2);

                dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
                dependencies[0].dstSubpass = 0;
                dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                dependencies[1].srcSubpass = 0;
                dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
                dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                VkRenderPassCreateInfo renderPassInfo = {};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.attachmentCount = static_cast<uint32_t>(pAttachments.size());
                renderPassInfo.pAttachments = pAttachments.data();
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpassDescription;
                renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
                renderPassInfo.pDependencies = dependencies.data();

                m_RenderPass.Initialize(m_Device, renderPassInfo);

                VkPipelineCacheCreateInfo pPipelineCacheCreateInfo {};
                pPipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
                m_PipelineCache.Initialize(m_Device, pPipelineCacheCreateInfo);

                std::vector<VkImageView> pFrameAttachments(2);
                pFrameAttachments[1] = m_ImageDepth;

                VkFramebufferCreateInfo pFrameBufferInfo{};
                pFrameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                pFrameBufferInfo.pNext = nullptr;
                pFrameBufferInfo.renderPass = m_RenderPass;
                pFrameBufferInfo.attachmentCount = 2;
                pFrameBufferInfo.pAttachments = pFrameAttachments.data();
                pFrameBufferInfo.width = m_ScreenWidth;
                pFrameBufferInfo.height = m_ScreenHeight;
                pFrameBufferInfo.layers = 1;

                m_FrameBuffers.resize(m_Swapchain.GetImageCount());

                uint32_t i = 0;
                for ( std::vector<VKFrameBuffer>::iterator it = m_FrameBuffers.begin(); it != m_FrameBuffers.end(); ++it)
                {
                    pFrameAttachments[0] = m_Swapchain.GetBuffers()[i].imageView;
                    it->Initialize(m_Device, pFrameBufferInfo);
                }
            }
        }
    }
}
