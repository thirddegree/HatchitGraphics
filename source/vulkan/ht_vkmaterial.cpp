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

            using namespace Resource;

            VKMaterial::VKMaterial(Core::Guid ID) :
                m_device(VKRenderer::RendererInstance->GetVKDevice()),
                Core::RefCounted<VKMaterial>(std::move(ID)) {}

            bool VKMaterial::Initialize(const std::string& fileName)
            {
                Resource::MaterialHandle handle = Hatchit::Resource::Material::GetHandleFromFileName(fileName);

                if (!handle.IsValid())
                {
                    HT_ERROR_PRINTF("VKMaterial::Initialize Failed to load resource handle");
                    return false;
                }

                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Gather resources and handles
                m_pipeline = VKPipeline::GetHandle(handle->GetPipelinePath(), handle->GetPipelinePath());

                //Get render pass paths and construct handles
                std::vector<std::string> renderPassPaths = handle->GetRenderPassPaths();
                for (size_t i = 0; i < renderPassPaths.size(); i++)
                {
                    std::string renderPassPath = renderPassPaths[i];
                    VKRenderPassHandle renderPassHandle = VKRenderPass::GetHandle(renderPassPath, renderPassPath);
                    m_renderPasses.push_back(renderPassHandle.StaticCastHandle<RenderPassBase>());
                }

                //Get shader vars
               // m_shaderVariables = handle->GetShaderVariables();


                m_descriptorSetLayouts = renderer->GetVKRootLayoutHandle()->VKGetDescriptorSetLayouts();

                std::vector<Resource::Material::TexturePath> texturePaths = handle->GetTexturePaths();
                //Map layout location to file handle
                for (size_t i = 0; i < texturePaths.size(); i++)
                {
                    Resource::Material::TexturePath texturePath = texturePaths[i];

                    LayoutLocation location;
                    location.set = texturePath.set;
                    location.binding = texturePath.binding;

                    std::string filePath = texturePath.path;

                    VKTextureHandle textureHandle = VKTexture::GetHandle(filePath, filePath);

                    m_textureLocations.push_back(location);
                    m_textures.push_back(textureHandle.StaticCastHandle<Texture>());    

                    //Record which descriptor set layouts we need
                    m_materialLayouts.push_back(m_descriptorSetLayouts[location.set]);
                }

                VkDescriptorPool descriptorPool = renderer->GetVKDescriptorPool();

                VKPipelineHandle vkPipeline = m_pipeline.DynamicCastHandle<VKPipeline>();

                setupDescriptorSet(descriptorPool);

                return true;
            }

            VKMaterial::~VKMaterial() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                VkDescriptorPool descriptorPool = renderer->GetVKDescriptorPool();

                //Free descriptor sets
                uint32_t descriptorSetCount = static_cast<uint32_t>(m_materialSets.size());
                VkDescriptorSet* descriptorSets = m_materialSets.data();

                vkFreeDescriptorSets(m_device, descriptorPool, descriptorSetCount, descriptorSets);

                //Destroy unifrom blocks
                vkFreeMemory(m_device, m_uniformVSBuffer.memory, nullptr);
                vkDestroyBuffer(m_device, m_uniformVSBuffer.buffer, nullptr);
            }

            bool VKMaterial::VSetInt(std::string name, int data)
            {
                //static_cast<IntVariable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetFloat(std::string name, float data)
            {
                //static_cast<FloatVariable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetFloat3(std::string name, Math::Vector3 data)
            {
                //static_cast<Float3Variable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetFloat4(std::string name, Math::Vector4 data) 
            {
                //static_cast<Float4Variable*>(m_shaderVariables[name])->SetData(data);
                return true;
            }
            bool VKMaterial::VSetMatrix4(std::string name, Math::Matrix4 data) 
            {
                //Matrix4Variable* var = static_cast<Matrix4Variable*>(m_shaderVariables[name]);
                //var->SetData(data);
                return true;
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

            IPipelineHandle VKMaterial::GetPipeline()
            {
                return m_pipeline.StaticCastHandle<IPipeline>();
            }

            bool VKMaterial::VUpdate() 
            {
                //TODO: Figure out what data is going into what buffers

                //if (m_shaderVariables.size() == 0)
                //    return true;

                //uint8_t* pData;
                //
                //std::vector<Math::Matrix4> variableList;
                //
                //std::map <std::string, ShaderVariable*>::iterator it;
                //for (it = m_shaderVariables.begin(); it != m_shaderVariables.end(); it++)
                //    variableList.push_back(*(Math::Matrix4*)(it->second->GetData()));
                //
                //VkResult err = vkMapMemory(m_device, m_uniformVSBuffer.memory, 0, sizeof(m_shaderVariables), 0, (void**)&pData);
                //assert(!err);
                //
                //memcpy(pData, variableList.data(), sizeof(Math::Matrix4));
                //
                //vkUnmapMemory(m_device, m_uniformVSBuffer.memory);

                return true;
            }

            const void VKMaterial::BindMaterial(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout) const
            { 
                if (m_materialSets.size() <= 0)
                    return;

                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3, 
                    static_cast<uint32_t>(m_materialSets.size()), m_materialSets.data(), 0, nullptr);
            }

            bool VKMaterial::setupDescriptorSet(VkDescriptorPool descriptorPool)
            {
                if (m_materialLayouts.size() <= 0)
                    return true;

                VkResult err;

                //Setup the descriptor sets
                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = descriptorPool;
                allocInfo.descriptorSetCount = static_cast<uint32_t>(m_materialLayouts.size());
                allocInfo.pSetLayouts = m_materialLayouts.data();

                m_materialSets.resize(m_materialLayouts.size());
                err = vkAllocateDescriptorSets(m_device, &allocInfo, m_materialSets.data());
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
                    VKTextureHandle texture = m_textures[i].DynamicCastHandle<VKTexture>();

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

                vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descSetWrites.size()), descSetWrites.data(), 0, nullptr);

                return true;
            }
        }
    }
}
