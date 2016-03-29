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

#include <ht_pipeline.h>

#include <ht_vkrenderpass.h>
#include <ht_vkshader.h>

#include <ht_vulkan.h>

#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKPipeline : public IPipeline
            {
            public:
                VKPipeline(const VkRenderPass* renderPass);
                ~VKPipeline();

                //If we wanted to allow users to control blending states
                //void VSetColorBlendAttachments(ColorBlendState* colorBlendStates) override;

                /* Set the rasterization state for this pipeline
                * \param rasterState A struct containing rasterization options
                */
                void VSetRasterState(const RasterizerState& rasterState)        override;

                /* Set the multisampling state for this pipeline
                * \param multiState A struct containing multisampling options
                */
                void VSetMultisampleState(const MultisampleState& multiState)   override;

                /* Load a shader into a shader slot for the pipeline
                * \param shaderSlot The slot that you want the shader in; vertex, fragment etc.
                * \param shader A pointer to the shader that you want to load to the given shader slot
                */
                void VLoadShader(ShaderSlot shaderSlot, IShader* shader)        override;

                bool VSetInt(std::string name, int data)                        override;
                bool VSetFloat(std::string name, float data)                    override;
                bool VSetFloat3(std::string name, Math::Vector3 data)           override;
                bool VSetFloat4(std::string name, Math::Vector4 data)           override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data)          override;

                ///Have Vulkan create a pipeline with these settings
                bool VPrepare()                                                 override;

                ///Have Vulkan update the descriptor sets in this pipeline
                bool VUpdate()                                                  override;

                //TODO: Remove this when we can just reflect shaders instead
                void SetVKDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

                VkPipeline                          GetVKPipeline();
                VkPipelineLayout                    GetVKPipelineLayout();
                std::vector<VkDescriptorSetLayout>  GetVKDescriptorSetLayouts();
                
                void SendPushConstants(VkCommandBuffer commandBuffer);

            protected:
                //Input
                const VkRenderPass* m_renderPass;

                VkPipelineRasterizationStateCreateInfo m_rasterizationState;
                VkPipelineMultisampleStateCreateInfo m_multisampleState;
                std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

                bool useGivenLayout = false;
                std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts; //0 is this pipeline set layout, 1 is the material set layout
                VkPipelineLayout        m_pipelineLayout;

                VkPipelineCache m_pipelineCache;
                VkPipeline      m_pipeline;

                std::vector<int>    m_intPushData;
                std::vector<float>  m_floatPushData;
                std::vector<float>  m_vector2PushData;
                std::vector<float>  m_vector3PushData;
                std::vector<float>  m_vector4PushData;
                std::vector<float>  m_matrixPushData;

            private:
                bool prepareLayouts(VkDevice device);
                bool preparePipeline(VkDevice device);
            };
        }
    }
}