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
#include <ht_vktexture.h>
#include <ht_vkrenderer.h>
#include <ht_vkpipeline.h>

#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKMaterial::VKMaterial() 
            { 
                //TODO: Allocate variables based on the material file
                m_shaderVariables["object.model"] = new Matrix4Variable();

                static_cast<Matrix4Variable*>(m_shaderVariables["object.model"])->SetData(Math::Matrix4());
            }
            VKMaterial::~VKMaterial() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                VkDevice device = renderer->GetVKDevice();
                VkDescriptorPool descriptorPool = renderer->GetVKDescriptorPool();

                //Free descriptors
                vkFreeDescriptorSets(device, descriptorPool, 1, &m_materialSet);

                //Destroy unifrom blocks
                vkFreeMemory(device, m_uniformVSBuffer.memory, nullptr);
                vkDestroyBuffer(device, m_uniformVSBuffer.buffer, nullptr);

                //TODO: Destroy FS buffer

            }

            bool VKMaterial::VInitFromFile(Core::File* file) { return true; }

            bool VKMaterial::VSetInt(std::string name, int data)
            {
                static_cast<IntVariable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetFloat(std::string name, float data)
            {
                static_cast<FloatVariable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetFloat3(std::string name, Math::Vector3 data)
            {
                static_cast<Float3Variable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetFloat4(std::string name, Math::Vector4 data) 
            {
                static_cast<Float4Variable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetMatrix4(std::string name, Math::Matrix4 data) 
            {
                Matrix4Variable* var = static_cast<Matrix4Variable*>(m_shaderVariables[name]);
                var->SetData(data);
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

            bool VKMaterial::VPrepare(IPipeline* pipeline) 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;
                VkDevice device = renderer->GetVKDevice();
                VkDescriptorPool descriptorPool = renderer->GetVKDescriptorPool();

                m_pipeline = pipeline;
                VKPipeline* vkPipeline = static_cast<VKPipeline*>(m_pipeline);

                m_materialLayout = vkPipeline->GetVKDescriptorSetLayouts()[1];

                //Prepare uniform buffers
                //if(m_shaderVariables.size() > 0)
                    renderer->CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Math::Matrix4), nullptr, &m_uniformVSBuffer);
                //renderer->CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 0, nullptr, &m_uniformFSBuffer);

                m_uniformVSBuffer.descriptor.offset = 0;
                m_uniformVSBuffer.descriptor.range = sizeof(Math::Matrix4);

                if (!setupDescriptorSet(descriptorPool, device))
                    return false;

                return true;
            }

            bool VKMaterial::VUpdate() 
            {
                //TODO: Figure out what data is going into what buffers

                if (m_shaderVariables.size() == 0)
                    return true;

                VkDevice device = VKRenderer::RendererInstance->GetVKDevice();

                uint8_t* pData;
                
                std::vector<Math::Matrix4> variableList;

                std::map <std::string, ShaderVariable*>::iterator it;
                for (it = m_shaderVariables.begin(); it != m_shaderVariables.end(); it++)
                    variableList.push_back(*(Math::Matrix4*)(it->second->GetData()));
                
                VkResult err = vkMapMemory(device, m_uniformVSBuffer.memory, 0, sizeof(m_shaderVariables), 0, (void**)&pData);
                assert(!err);
                
                memcpy(pData, variableList.data(), sizeof(Math::Matrix4));

                vkUnmapMemory(device, m_uniformVSBuffer.memory);

                return true;
            }

            VkDescriptorSet* VKMaterial::GetVKDescriptorSet() { return &m_materialSet; }

            bool VKMaterial::setupDescriptorSet(VkDescriptorPool descriptorPool, VkDevice device)
            {
                VkResult err;

                //Setup the descriptor sets
                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = descriptorPool;
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
                uint32_t descCount = 0;

                VkWriteDescriptorSet uniformVSWrite = {};
                uniformVSWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                uniformVSWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformVSWrite.dstSet = m_materialSet;
                uniformVSWrite.dstBinding = descCount++;
                uniformVSWrite.pBufferInfo = &m_uniformVSBuffer.descriptor;
                uniformVSWrite.descriptorCount = 1;

                //VkWriteDescriptorSet uniformFSWrite = {};
                //uniformFSWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                //uniformFSWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                //uniformFSWrite.dstSet = m_materialSet;
                //uniformFSWrite.dstBinding = descCount++;
                //uniformFSWrite.pBufferInfo = &m_uniformFSBuffer.descriptor;
                //uniformFSWrite.descriptorCount = 1;

                descSetWrites.push_back(uniformVSWrite);
                //descSetWrites.push_back(uniformFSWrite);

                //Setup writes for textures
                std::map<std::string, ITexture*>::iterator it;
                for (it = m_textures.begin(); it != m_textures.end(); it++)
                {
                    VKTexture* texture = static_cast<VKTexture*>(it->second);

                    //Create Texture description
                    VkDescriptorImageInfo textureDescriptor = {};
                    textureDescriptor.sampler = texture->GetSampler();
                    textureDescriptor.imageView = texture->GetView();
                    textureDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                    VkWriteDescriptorSet samplerFSWrite = {};
                    samplerFSWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    samplerFSWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    samplerFSWrite.dstSet = m_materialSet;
                    samplerFSWrite.dstBinding = descCount++;
                    samplerFSWrite.pImageInfo = &textureDescriptor;
                    samplerFSWrite.descriptorCount = 1;

                    descSetWrites.push_back(samplerFSWrite);
                }

                vkUpdateDescriptorSets(device, static_cast<uint32_t>(descSetWrites.size()), descSetWrites.data(), 0, nullptr);

                return true;
            }
        }
    }
}
