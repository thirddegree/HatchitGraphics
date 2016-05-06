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
#include <ht_renderpass.h>
#include <ht_vkpipeline.h>
#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            using namespace Resource;

            VKMaterial::VKMaterial() {}

            bool VKMaterial::Initialize(Resource::MaterialHandle handle, const VkDevice& device, const VkDescriptorPool& descriptorPool)
            {
                m_device = &device;
                m_descriptorPool = &descriptorPool;

                if (!handle.IsValid())
                {
                    HT_ERROR_PRINTF("VKMaterial::Initialize Failed to load resource handle");
                    return false;
                }

                //Gather resources and handles
                std::string pipelinePath = handle->GetPipelinePath();

                m_pipelineHandle = Pipeline::GetHandle(pipelinePath, pipelinePath);
                m_pipeline = static_cast<VKPipeline*>(m_pipelineHandle->GetBase());

                //Get render pass paths and construct handles
                std::vector<std::string> renderPassPaths = handle->GetRenderPassPaths();
                for (size_t i = 0; i < renderPassPaths.size(); i++)
                {
                    std::string renderPassPath = renderPassPaths[i];
                    RenderPassHandle renderPassHandle = RenderPass::GetHandle(renderPassPath, renderPassPath);
                    m_renderPasses.push_back(renderPassHandle);
                }

                //Get shader vars
                std::vector<Resource::Material::ShaderVariableBinding> bindings = handle->GetShaderVariables();
                for (size_t i = 0; i < bindings.size(); i++)
                {
                    //create a layout location for the variable chunk
                    LayoutLocation location;
                    location.set = bindings[i].set;
                    location.binding = bindings[i].binding;
                    m_shaderVariableLocations.push_back(location);
                    //turn the variable map into a variable chunk
                    m_shaderVariables.push_back(new ShaderVariableChunk(bindings[i].shaderVariables));
                }
                
                //Get root layout from first render pass
                VKRenderPass* renderPass = static_cast<VKRenderPass*>(m_renderPasses[0]->GetBase());
                const VKRootLayout* rootLayout = renderPass->GetVKRootLayout();

                m_descriptorSetLayouts = rootLayout->VKGetDescriptorSetLayouts();

                std::vector<Resource::Material::TexturePath> texturePaths = handle->GetTexturePaths();
                //Map layout location to file handle
                for (size_t i = 0; i < texturePaths.size(); i++)
                {
                    Resource::Material::TexturePath texturePath = texturePaths[i];

                    LayoutLocation location;
                    location.set = texturePath.set;
                    location.binding = texturePath.binding;

                    std::string filePath = texturePath.path;

                    TextureHandle textureHandle = Texture::GetHandle(filePath, filePath);
                    m_textureHandles[filePath] = textureHandle; //Store handle so it doesn't de-ref and get deleted
                    VKTexture* texture = static_cast<VKTexture*>(textureHandle->GetBase());

                    m_textureLocations.push_back(location);
                    m_textures.push_back(texture);

                    //Record which descriptor set layouts we need
                    m_materialLayouts.push_back(m_descriptorSetLayouts[location.set]);
                }

                setupDescriptorSet();

                return true;
            }

            VKMaterial::~VKMaterial() 
            {
                //Free descriptor sets
                uint32_t descriptorSetCount = static_cast<uint32_t>(m_materialSets.size());
                VkDescriptorSet* descriptorSets = m_materialSets.data();

                vkFreeDescriptorSets(*m_device, *m_descriptorPool, descriptorSetCount, descriptorSets);

                //Destroy unifrom blocks
                //vkFreeMemory(*m_device, m_uniformVSBuffer.memory, nullptr);
                //vkDestroyBuffer(*m_device, m_uniformVSBuffer.buffer, nullptr);
            }


            bool VKMaterial::VBindTexture(std::string name, TextureHandle texture) 
            {
                //m_textures[name] = texture;
                return true;
            }
            bool VKMaterial::VUnbindTexture(std::string name, TextureHandle texture)
            {
                //m_textures.erase(name);
                return true;
            }

            PipelineHandle const VKMaterial::VGetPipeline() const 
            {
                return m_pipelineHandle;
            }

            const VKPipeline* VKMaterial::GetVKPipeline() const
            {
                return m_pipeline;
            }

            bool VKMaterial::VUpdate() 
            {
                //TODO: Figure out what data is going into what buffers
                /*if (m_shaderVariables.size() == 0)
                    return true;

                uint8_t* pData;
                std::vector<Math::Matrix4> variableList;

                for (size_t i = 0; i < m_shaderVariables.size(); i++)
                {
                    m_shaderVariables[i]->GetByteData();
                    m_shaderVariables[i]->GetSize();
                }

                VkResult err = vkMapMemory(*m_device, m_uniformVSBuffer.memory, 0, sizeof(m_shaderVariables), 0, (void**)&pData);
                assert(!err);
                
                memcpy(pData, variableList.data(), size);
                
                vkUnmapMemory(*m_device, m_uniformVSBuffer.memory);

                return true;*/
            }

            const void VKMaterial::BindMaterial(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout) const
            { 
                if (m_materialSets.size() <= 0)
                    return;

                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3, 
                    static_cast<uint32_t>(m_materialSets.size()), m_materialSets.data(), 0, nullptr);
            }

            bool VKMaterial::setupDescriptorSet()
            {
                if (m_materialLayouts.size() <= 0)
                    return true;

                VkResult err;

                //Setup the descriptor sets
                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = *m_descriptorPool;
                allocInfo.descriptorSetCount = static_cast<uint32_t>(m_materialLayouts.size());
                allocInfo.pSetLayouts = m_materialLayouts.data();

                m_materialSets.resize(m_materialLayouts.size());
                err = vkAllocateDescriptorSets(*m_device, &allocInfo, m_materialSets.data());
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKMaterial::VPrepare: Failed to allocate descriptor set\n");
                    return false;
                }

                std::vector<VkWriteDescriptorSet> descSetWrites = {};
                uint32_t writeCount = 0;


                //Setup writes for textures
                for (size_t i = 0; i < m_textures.size(); i++)
                {
                    LayoutLocation location = m_textureLocations[i];
                    VKTexture* texture = m_textures[i];

                    //Create Texture description
                    VkDescriptorImageInfo textureDescriptor = {};
                    textureDescriptor.sampler = nullptr; //Sampler applied in shader
                    textureDescriptor.imageView = texture->GetView();
                    textureDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                    VkWriteDescriptorSet samplerFSWrite = {};
                    samplerFSWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    samplerFSWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    samplerFSWrite.dstSet = m_materialSets[writeCount++];
                    samplerFSWrite.dstBinding = location.binding;
                    samplerFSWrite.pImageInfo = &textureDescriptor;
                    samplerFSWrite.descriptorCount = 1;

                    descSetWrites.push_back(samplerFSWrite);
                }

                vkUpdateDescriptorSets(*m_device, static_cast<uint32_t>(descSetWrites.size()), descSetWrites.data(), 0, nullptr);

                return true;
            }
        }
    }
}
