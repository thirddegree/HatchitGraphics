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

#include <ht_vkrootlayout.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKRootLayout::VKRootLayout(const VkDevice& device) :
                m_device(device)
            {
                m_pipelineLayout = VK_NULL_HANDLE;
            }

            VKRootLayout::~VKRootLayout() {}

            bool VKRootLayout::VInitialize(const Resource::RootLayoutHandle handle) 
            {
                using namespace Resource;

                VkResult err;

                if (!handle.IsValid())
                    return false;

                std::vector<RootLayout::Parameter> parameters = handle->GetParameters();

                for (uint32_t i = 0; i < handle->GetParameterCount(); i++)
                {
                    RootLayout::Parameter p = parameters[i];

                    //Will create descriptorSet to go into m_descriptorSetLayouts
                    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
                    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    descriptorSetLayoutInfo.pNext = nullptr;
                    descriptorSetLayoutInfo.flags = 0;
                    
                    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

                    //Determine visibility
                    VkShaderStageFlags shaderStages;

                    switch (p.visibility)
                    {
                    case RootLayout::ShaderVisibility::ALL:
                        shaderStages = VK_SHADER_STAGE_ALL_GRAPHICS;
                        break;
                    case RootLayout::ShaderVisibility::VERTEX:
                        shaderStages = VK_SHADER_STAGE_VERTEX_BIT;
                        break;
                    case RootLayout::ShaderVisibility::FRAGMENT:
                        shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    case RootLayout::ShaderVisibility::GEOMETRY:
                        shaderStages = VK_SHADER_STAGE_GEOMETRY_BIT;
                        break;
                    case RootLayout::ShaderVisibility::TESS_CONTROL:
                        shaderStages = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                        break;
                    case RootLayout::ShaderVisibility::TESS_EVAL:
                        shaderStages = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                        break;
                    }

                    switch (p.type)
                    {
                        case RootLayout::Parameter::Type::TABLE:
                        {
                            std::vector<RootLayout::Range> ranges = p.data.table.ranges;
                            for (uint32_t j = 0; j < p.data.table.rangeCount; j++)
                            {
                                RootLayout::Range r = ranges[j];

                                //Determine binding type
                                VkDescriptorType descType;

                                switch (r.type)
                                {
                                    //TODO: Figure out how to use other types
                                case RootLayout::Range::Type::CONSTANT_BUFFER:
                                    descType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                    break;
                                case RootLayout::Range::Type::SHADER_RESOURCE:
                                    descType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                                    break;
                                }

                                VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
                                descriptorSetLayoutBinding.binding = j;
                                descriptorSetLayoutBinding.descriptorCount = r.numDescriptors;
                                descriptorSetLayoutBinding.descriptorType = descType;
                                descriptorSetLayoutBinding.stageFlags = shaderStages;

                                descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
                            }

                            //Finish the layout info by describing how many bindings we want in this set
                            descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
                            descriptorSetLayoutInfo.pBindings = descriptorSetLayoutBindings.data();

                            VkDescriptorSetLayout descriptorSetLayout;
                            err = vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout);
                            assert(!err);
                            if (err != VK_SUCCESS)
                            {
                                HT_DEBUG_PRINTF("VKRootLayout::VInitialize(): Error creating descriptor set layout!");
                                return false;
                            }

                            m_descriptorSetLayouts.push_back(descriptorSetLayout);
                        } 
                        break;
                    }

                }

                //Use all the defined push constants and descriptor layouts to make the pipeline layout
                VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
                pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.pNext = nullptr;
                pipelineLayoutInfo.flags = 0;
                pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
                pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayouts.data();
                //pipelineLayoutInfo.

                err = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRootLayout::VInitialize(): Error creating pipeline layout!");
                    return false;
                }

                return true;
            }

            const VkPipelineLayout& VKRootLayout::VKGetPipelineLayout() const { return m_pipelineLayout; }
            std::vector<VkDescriptorSetLayout> VKRootLayout::VKGetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }
            std::vector<VkPushConstantRange> VKRootLayout::VKGetPushConstantRanges() const { return m_pushConstantRanges;  }
        }
    }
}