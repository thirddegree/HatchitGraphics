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
* \class VKPipeline
* \ingroup HatchitGraphics
*
* \brief An extension of IPipeline implemented with Vulkan
*
* Creates a pipeline with Vulkan that controls rasterization state, 
* topology, shaders etc.
*/

#pragma once

#include <ht_pipeline_base.h>

#include <ht_vkrenderpass.h>
#include <ht_vkshader.h>

#include <ht_vulkan.h>

#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKPipeline : public PipelineBase
            {
            public:
                VKPipeline();
                virtual ~VKPipeline();

                bool Initialize(const Resource::PipelineHandle& handle, const VkDevice& device, const VkDescriptorPool& descriptorPool);

                ///Have Vulkan update the descriptor sets in this pipeline
                bool VUpdate()                                                  override;

                /* Add a map of existing shader variables into this pipeline
                * \param shaderVariables the map of existing shader variables you want to add
                */
                bool VSetShaderVariables(ShaderVariableChunk* variables) override;

                bool VSetInt(size_t offset, int data)                override;
                bool VSetDouble(size_t offset, double data)          override;
                bool VSetFloat(size_t offset, float data)            override;
                bool VSetFloat2(size_t offset, Math::Vector2 data)   override;
                bool VSetFloat3(size_t offset, Math::Vector3 data)   override;
                bool VSetFloat4(size_t offset, Math::Vector4 data)   override;
                bool VSetMatrix4(size_t offset, Math::Matrix4 data)  override;

                VkPipeline                          GetVKPipeline();
                
                void BindPipeline(const VkCommandBuffer& commandBuffer);

            protected:
                //Input
                VkDevice m_device;
                VkDescriptorPool m_descriptorPool;
                VKRenderPass* m_renderPass;

                std::vector<VkVertexInputAttributeDescription> m_vertexLayout;

                uint32_t m_vertexLayoutStride;
                uint32_t m_instanceLayoutStride;

                VkPipelineDepthStencilStateCreateInfo m_depthStencilState;
                VkPipelineRasterizationStateCreateInfo m_rasterizationState;
                VkPipelineMultisampleStateCreateInfo m_multisampleState;
                
                std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
                std::map<Resource::Pipeline::ShaderSlot, Graphics::ShaderHandle> m_shaderHandles;

                VkPipelineCache     m_pipelineCache;
                VkPipelineLayout    m_pipelineLayout; //Given by the root layout
                VkPipeline          m_pipeline;

                std::vector<BYTE> m_pushData;
                std::vector<BYTE> m_descriptorData;

                UniformBlock_vk m_uniformVSBuffer;
                uint8_t* m_uniformBindPoint;
                VkDescriptorSet m_descriptorSet; //Descriptor set for data that can't fit into push constants

            private:
                bool m_hasVertexAttribs;
                bool m_hasIndexAttribs;

                VKRootLayout* m_rootLayout;

                /* Set the vertex layout
                * \param vertexLayout A vector of all of the vertex attributes in this layout
                */
                void setVertexLayout(const std::vector<Resource::Pipeline::Attribute> vertexLayout);

                /* Set the instance layout
                * \param instanceLayout A vector of all of the instance attributes in this layout
                */
                void setInstanceLayout(const std::vector<Resource::Pipeline::Attribute> instanceLayout);

                void setDepthStencilState(const Hatchit::Resource::Pipeline::DepthStencilState& depthStencilState);

                /* Set the rasterization state for this pipeline
                * \param rasterState A struct containing rasterization options
                */
                void setRasterState(const Hatchit::Resource::Pipeline::RasterizerState& rasterState);

                /* Set the multisampling state for this pipeline
                * \param multiState A struct containing multisampling options
                */
                void setMultisampleState(const Hatchit::Resource::Pipeline::MultisampleState& multiState);

                /* Load a shader into a shader slot for the pipeline
                * \param shaderSlot The slot that you want the shader in; vertex, fragment etc.
                * \param shader A pointer to the shader that you want to load to the given shader slot
                */
                void loadShader(Hatchit::Resource::Pipeline::ShaderSlot shaderSlot, Graphics::ShaderHandle shader);

                bool preparePipeline();

                bool prepareDescriptorSet();

                VkFormat formatFromType(const Resource::ShaderVariable::Type& type) const;

                void addAttributesToLayout(const std::vector<Resource::Pipeline::Attribute>& attributes, std::vector<VkVertexInputAttributeDescription>& vkAttributes, uint32_t& outStride);

                VkBlendOp getVKBlendOpFromResourceBlendOp(Resource::RenderTarget::BlendOp blendOp);
            };
        }
    }
}