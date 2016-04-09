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
#include <ht_rootlayout.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKRootLayout : public IRootLayout
            {
            public:
                VKRootLayout(const VkDevice& device);

                ~VKRootLayout();

                bool VInitialize(const Resource::RootLayoutHandle handle) override;

                const VkPipelineLayout& VKGetPipelineLayout() const;
                std::vector<VkDescriptorSetLayout> VKGetDescriptorSetLayouts() const;
                std::vector<VkPushConstantRange> VKGetPushConstantRanges() const;

            private:
                const VkDevice& m_device;

                VkPipelineLayout m_pipelineLayout;
                std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
                std::vector<VkPushConstantRange> m_pushConstantRanges;
            };
        }
    }
}