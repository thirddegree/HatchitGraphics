
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

#pragma once

#include <ht_platform.h>
#include <ht_vkdevice.h>
#include <ht_vulkan.h>
#include <ht_vkapplication.h>
#include <ht_vkswapchain.h>
#include <ht_vkcommandpool.h>
#include <ht_vkcommandbuffer.h>
#include <ht_vkimage.h>
#include <ht_vkrenderpass.h>
#include <ht_vkpipelinecache.h>
#include <ht_vkframebuffer.h>
#include <ht_vkpipeline.h>
#include <ht_vkpipelinelayout.h>
#include <ht_vkdescriptorsetlayout.h>
#include <ht_vkdescriptorpool.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class Setup
            {
                struct VertexDesc
                {
                    VkPipelineVertexInputStateCreateInfo inputState;
                    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
                    std::vector<VkVertexInputAttributeDescription> attributeDescription;
                };
            public:
                Setup();
                ~Setup();
                /* Setup the vulkan app */
                void Initialize();
            protected:
                VKCommandBuffer m_CommandBuffer;
                VertexDesc m_VertexDescription;
                VKApplication m_Application;
                VKDevice m_Device;
                VKSwapChain m_Swapchain;
                VKCommandPool m_CommandPool;
                uint32_t m_ScreenHeight;
                uint32_t m_ScreenWidth;
                VKImage m_ImageDepth;
                //VKRenderPass m_RenderPass;
                VKPipelineCache m_PipelineCache;
                VKPipelineLayout m_PipelineLayout;
                VKDescriptorSetLayout m_DescSetLayout;
                VKDescriptorPool m_DescPool;
                VKPipeline m_Pipeline;
                std::vector<VKFrameBuffer> m_FrameBuffers;
            };
        }
    }
}
