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

#include <ht_vkmaterial.h>
#include <ht_vkshader.h>
#include <ht_vkrenderer.h>

#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKMaterial::VKMaterial() { }
            VKMaterial::~VKMaterial() {}

            void VKMaterial::VOnLoaded()
            {
                //TODO: Read the material file and set the appropriate shader variables
            }
            bool VKMaterial::VInitFromFile(Core::File* file) { return true; }

            bool VKMaterial::VSetInt(std::string name, int data)
            {
                m_shaderVariables[name] = IntVariable(data);
                return true;
            }
            bool VKMaterial::VSetFloat(std::string name, float data)
            {
                m_shaderVariables[name] = FloatVariable(data);
                return true;
            }
            bool VKMaterial::VSetFloat3(std::string name, Math::Vector3 data)
            {
                m_shaderVariables[name] = Float3Variable(data);
                return true;
            }
            bool VKMaterial::VSetFloat4(std::string name, Math::Vector4 data) 
            {
                m_shaderVariables[name] = Float4Variable(data);
                return true;
            }
            bool VKMaterial::VSetMatrix4(std::string name, Math::Matrix4 data) 
            {
                m_shaderVariables[name] = Matrix4Variable(data);
                return true;
            }

            bool VKMaterial::VBindTexture(std::string name, ITexture* texture) 
            {
                m_textures[name] = texture;
                return true;
            }
            bool VKMaterial::VUnbindTexture(std::string name, ITexture* texture)
            {
                m_textures.erase(name);
                return true;
            }

            bool VKMaterial::VPrepare() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;
                VkDevice device = renderer->GetVKDevice();

                //Prepare uniform buffers
                renderer->CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 128, nullptr, &m_uniformVSBuffer);
                renderer->CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 128, nullptr, &m_uniformFSBuffer);

                if (!setupDescriptorSetLayout(device))
                    return false;

                if (!setupDescriptorPool(device))
                    return false;

                if (!setupDescriptorSet(device))
                    return false;

                return true;
            }

            bool VKMaterial::VUpdate() 
            {
                //TODO: Figure out what data is going into what buffers

                return true;
            }

            VkDescriptorSetLayout VKMaterial::GetDescriptorSetLayout() { return m_materialLayout; }
            VkDescriptorSet VKMaterial::GetDescriptorSet() { return m_materialSet; }

            bool VKMaterial::setupDescriptorSetLayout(VkDevice device)
            {
                VkResult err;

                //Only one layout for vertex layout
                std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {};

                VkDescriptorSetLayoutBinding vertexUniformBinding = {};
                vertexUniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                vertexUniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                vertexUniformBinding.binding = 0;
                vertexUniformBinding.descriptorCount = 1;

                VkDescriptorSetLayoutBinding fragmentUniformBinding = {};
                fragmentUniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                fragmentUniformBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                fragmentUniformBinding.binding = 1;
                fragmentUniformBinding.descriptorCount = 1;

                layoutBindings.push_back(vertexUniformBinding);
                layoutBindings.push_back(fragmentUniformBinding);

                for (uint32_t i = 0; i < m_textures.size(); i++)
                {
                    VkDescriptorSetLayoutBinding fragmentTextureBinding = {};
                    fragmentTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    fragmentTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    fragmentTextureBinding.binding = 2 + i;
                    fragmentTextureBinding.descriptorCount = 1;
                    layoutBindings.push_back(fragmentTextureBinding);
                }

                VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
                descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
                descriptorLayoutInfo.pBindings = layoutBindings.data();

                err = vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr, &m_materialLayout);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMaterial::VPrepare: Failed to create descriptor set layout\n");
#endif
                    return false;
                }

                return true;
            }
            bool VKMaterial::setupDescriptorPool(VkDevice device)
            {
                VkResult err;
                
                //Setup the descriptor pool
                std::vector<VkDescriptorPoolSize> poolSizes;

                VkDescriptorPoolSize uniformSize = {};
                uniformSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformSize.descriptorCount = 2;
                poolSizes.push_back(uniformSize);

                if (m_textures.size() > 0)
                {
                    VkDescriptorPoolSize samplerSize = {};
                    samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    samplerSize.descriptorCount = static_cast<uint32_t>(m_textures.size());
                    poolSizes.push_back(samplerSize);
                }

                VkDescriptorPoolCreateInfo poolCreateInfo = {};
                poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolCreateInfo.pPoolSizes = poolSizes.data();
                poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
                poolCreateInfo.maxSets = static_cast<uint32_t>(2 + m_textures.size());

                err = vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &m_descriptorPool);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMaterial::VPrepare: Failed to create descriptor pool\n");
#endif
                    return false;
                }

                return true;
            }
            bool VKMaterial::setupDescriptorSet(VkDevice device)
            {
                VkResult err;

                //Setup the descriptor sets
                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = m_descriptorPool;
                allocInfo.pSetLayouts = &m_materialLayout;
                allocInfo.descriptorSetCount = 1;

                err = vkAllocateDescriptorSets(device, &allocInfo, &m_materialSet);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMaterial::VPrepare: Failed to allocate descriptor set\n");
#endif
                    return false;
                }

                std::vector<VkWriteDescriptorSet> descSetWrites = {};

                VkWriteDescriptorSet uniformVSWrite = {};
                uniformVSWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                uniformVSWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformVSWrite.dstSet = m_materialSet;
                uniformVSWrite.dstBinding = 0;
                uniformVSWrite.pBufferInfo = &m_uniformVSBuffer.descriptor;
                uniformVSWrite.descriptorCount = 1;

                VkWriteDescriptorSet uniformFSWrite = {};
                uniformFSWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                uniformFSWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformFSWrite.dstSet = m_materialSet;
                uniformFSWrite.dstBinding = 1;
                uniformFSWrite.pBufferInfo = &m_uniformFSBuffer.descriptor;
                uniformFSWrite.descriptorCount = 1;

                //TODO: Figure out the writes for the textures

                descSetWrites.push_back(uniformVSWrite);
                descSetWrites.push_back(uniformFSWrite);

                vkUpdateDescriptorSets(device, static_cast<uint32_t>(descSetWrites.size()), descSetWrites.data(), 0, nullptr);

                return true;
            }
        }
    }
}
