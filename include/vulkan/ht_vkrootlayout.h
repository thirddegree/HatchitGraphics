/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 ThirdDegree
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
#include <ht_vulkan.h>
#include <ht_rootlayout_base.h>
#include <ht_rootlayout_resource.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKSampler;

            class HT_API VKRootLayout : public RootLayoutBase
            {
            public:
                VKRootLayout();

                ~VKRootLayout();

                bool Initialize(const Resource::RootLayoutHandle& handle, const VkDevice& device, const VkDescriptorPool& descriptorPool);

                const VkPipelineLayout& VKGetPipelineLayout() const;
                const VkDescriptorSet& VKGetSamplerSet() const;
                std::vector<VkDescriptorSetLayout> VKGetDescriptorSetLayouts() const;
                std::vector<VkPushConstantRange> VKGetPushConstantRanges() const;

            private:
                VkDevice m_device;

                std::vector<VKSampler*> m_samplers; //So we can delete them later
                VkDescriptorSet         m_samplerSet; //Bind this so we can avoid the pipeline complaining about it
                VkPipelineLayout m_pipelineLayout;
                std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
                std::vector<VkPushConstantRange> m_pushConstantRanges;

                bool setupSamplerSet(const VkDevice& device, const VkDescriptorPool& descriptorPool);
            };
        }
    }
}