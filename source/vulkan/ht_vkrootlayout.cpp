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
#include <ht_vksampler.h>
#include <ht_rootlayout_resource.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKRootLayout::VKRootLayout()
            {
                m_device = VK_NULL_HANDLE;

                m_pipelineLayout = VK_NULL_HANDLE;
                m_samplerSet = VK_NULL_HANDLE;
            }

            VKRootLayout::~VKRootLayout() 
            {
                if (m_device != VK_NULL_HANDLE)
                {
                    //Destroy samplers
                    for (size_t i = 0; i < m_samplers.size(); i++)
                        delete m_samplers[i];

                    //Destroy descriptor set layouts
                    for (size_t i = 0; i < m_descriptorSetLayouts.size(); i++)
                        vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayouts[i], nullptr);

                    //Destroy pipeline layout
                    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
                }
                else 
                {
                    HT_DEBUG_PRINTF("VKRootLayout::~VKRootLayout: Tried to destroy Vulkan objects but VkDevice was a NULL HANDLE\n");
                }
            }

            bool VKRootLayout::Initialize(const Resource::RootLayoutHandle& handle, const VkDevice& device, const VkDescriptorPool& descriptorPool)
            {
                using namespace Resource;

                if (!handle.IsValid())
                    return false;

                m_device = device; //Save device for destruction later

                VkResult err;

                //The first entry in m_descriptorSetLayouts will be this descriptor set layout
                //It will contain all immutable samplers
                VkDescriptorSetLayoutCreateInfo immutableSamplersSetLayoutCreateInfo = {};
                immutableSamplersSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                immutableSamplersSetLayoutCreateInfo.pNext = nullptr;
                immutableSamplersSetLayoutCreateInfo.flags = 0;
                std::vector<VkDescriptorSetLayoutBinding> immutableBindings;

                //Parse immutable samplers
                std::vector<Resource::Sampler> samplers = handle->GetSamplers();
                std::vector<VkSampler> vkSamplers;
                for (size_t i = 0; i < samplers.size(); i++)
                {
                    Resource::Sampler sampler = samplers[i];

                    VKSampler* vkSampler = new VKSampler();
                    vkSampler->InitFromResource(sampler, device);
                    m_samplers.push_back(vkSampler);

                    vkSamplers.push_back(vkSampler->GetVkSampler());
                }

                //One binding for immutable samplers
                VkDescriptorSetLayoutBinding samplerBinding = {};
                samplerBinding.binding = 0;
                samplerBinding.descriptorCount = static_cast<uint32_t>(vkSamplers.size());
                samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                samplerBinding.stageFlags = VK_SHADER_STAGE_ALL;
                samplerBinding.pImmutableSamplers = vkSamplers.data();

                immutableBindings.push_back(samplerBinding);

                immutableSamplersSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(immutableBindings.size());
                immutableSamplersSetLayoutCreateInfo.pBindings = immutableBindings.data();

                VkDescriptorSetLayout immutableSamplersSetLayout;

                err = vkCreateDescriptorSetLayout(device, &immutableSamplersSetLayoutCreateInfo, nullptr, &immutableSamplersSetLayout);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKRootLayout::VInitialize(): Error creating descriptor set layout!");
                    return false;
                }

                m_descriptorSetLayouts.push_back(immutableSamplersSetLayout);

                //Parse layout parameters
                std::vector<RootLayout::Parameter> parameters = handle->GetParameters();
                uint32_t currentPushContentOffset = 0; //How many bytes the next push constant should be offset by

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
                    case RootLayout::ShaderVisibility::UNKNOWN:
                        break;
                    }

                    switch (p.type)
                    {
                        case RootLayout::Parameter::Type::UNKNOWN:
                        case RootLayout::Parameter::Type::CONSTANT_BUFFER:
                        case RootLayout::Parameter::Type::SHADER_RESOURCE:
                        case RootLayout::Parameter::Type::UNORDERED_ACCESS:
                            break;

                        //Descriptor Set Layouts
                        case RootLayout::Parameter::Type::TABLE:
                        {
                            //Each "Range" is a descriptor type
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
                                case RootLayout::Range::Type::UNORDERED_ACCESS:
                                    descType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                                    break;
                                case RootLayout::Range::Type::SHADER_RESOURCE:
                                    descType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                                    break;
                                case RootLayout::Range::Type::SAMPLER:
                                case RootLayout::Range::Type::UNKNOWN:
                                     break;
                                }

                                VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
                                descriptorSetLayoutBinding.binding = r.baseRegister;
                                descriptorSetLayoutBinding.descriptorCount = r.numDescriptors;
                                descriptorSetLayoutBinding.descriptorType = descType;
                                descriptorSetLayoutBinding.stageFlags = shaderStages;

                                descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
                            }

                            //Finish the layout info by describing how many bindings we want in this set
                            descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
                            descriptorSetLayoutInfo.pBindings = descriptorSetLayoutBindings.data();

                            VkDescriptorSetLayout descriptorSetLayout;
                            err = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout);
                            assert(!err);
                            if (err != VK_SUCCESS)
                            {
                                HT_DEBUG_PRINTF("VKRootLayout::VInitialize(): Error creating descriptor set layout!");
                                return false;
                            }

                            m_descriptorSetLayouts.push_back(descriptorSetLayout);
                        } 
                        break;

                        //Push Constants
                        case RootLayout::Parameter::Type::CONSTANTS:
                        {
                            VkPushConstantRange pushConstantRange;
                            pushConstantRange.stageFlags = shaderStages;
                            pushConstantRange.offset = currentPushContentOffset;
                            
                            //Size is based off reported constant type and must be a multiple of 4
                            size_t size;
                            ShaderVariable::Type type = p.data.constant.type;

                            size = ShaderVariable::SizeFromType(type);
                            
                            //Size is also based off of the value count
                            size *= p.data.constant.valueCount;
                            pushConstantRange.size = static_cast<uint32_t>(size);
                            currentPushContentOffset += static_cast<uint32_t>(size); //Increment offset for the next possible push constant

                            m_pushConstantRanges.push_back(pushConstantRange);
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
                pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(m_pushConstantRanges.size());
                pipelineLayoutInfo.pPushConstantRanges = m_pushConstantRanges.data();
                
                err = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKRootLayout::Initialize(): Could not create pipeline layout!\n");
                    return false;
                }

                if (!setupSamplerSet(device, descriptorPool))
                {
                    HT_ERROR_PRINTF("VKRootLayout::Initialize(): Could not create sampler descriptor set!\n");
                    return false;
                }

                return true;
            }

            const VkPipelineLayout& VKRootLayout::VKGetPipelineLayout() const { return m_pipelineLayout; }
            const VkDescriptorSet& VKRootLayout::VKGetSamplerSet() const { return m_samplerSet; }
            std::vector<VkDescriptorSetLayout> VKRootLayout::VKGetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }
            std::vector<VkPushConstantRange> VKRootLayout::VKGetPushConstantRanges() const { return m_pushConstantRanges;  }

            bool VKRootLayout::setupSamplerSet(const VkDevice& device, const VkDescriptorPool& descriptorPool) 
            {
                VkResult err;

                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.pNext = nullptr;
                allocInfo.descriptorPool = descriptorPool;
                allocInfo.descriptorSetCount = 1;
                allocInfo.pSetLayouts = &m_descriptorSetLayouts[0];

                err = vkAllocateDescriptorSets(device, &allocInfo, &m_samplerSet);
                assert(!err);
                if (err != VK_SUCCESS)
                    return false;

                std::vector<VkDescriptorImageInfo> samplerInfo;
                for (size_t i = 0; i < m_samplers.size(); i++)
                {
                    VkDescriptorImageInfo info = {};
                    info.sampler = m_samplers[i]->GetVkSampler();
                    samplerInfo.push_back(info);
                }

                VkWriteDescriptorSet write = {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext = nullptr;
                write.descriptorCount = static_cast<uint32_t>(samplerInfo.size());
                write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                write.pImageInfo = samplerInfo.data();
                write.dstSet = m_samplerSet;
                write.dstBinding = 0;
                
                vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);

                return true;
            }
        }
    }
}
