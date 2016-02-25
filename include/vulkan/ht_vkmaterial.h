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
* \class VKMaterial
* \ingroup HatchitGraphics
*
* \brief A collection of shader variables handled by Vulkan
*
* After creating a material and setting its shader variables
* this class will build a VkDescriptorSet to describe what
* will be sent to the GPU.
*/

#pragma once

#include <ht_material.h>

#include <ht_vkrenderpass.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKMaterial : IMaterial
            {
            public:
                VKMaterial(VKRenderPass* renderPass);
                ~VKMaterial();

                ///Callback for when this VKMaterial has been loaded from the disk. Used to build shader variable list
                void VOnLoaded()                                            override;
                bool VInitFromFile(Core::File* file)                        override;

                bool VSetInt(std::string name, int data)                    override;
                bool VSetFloat(std::string name, float data)                override;
                bool VSetFloat2(std::string name, Math::Vector2 data)       override;
                bool VSetFloat3(std::string name, Math::Vector3 data)       override;
                bool VSetFloat4(std::string name, Math::Vector4 data)       override;
                bool VSetMatrix3(std::string name, Math::Matrix3 data)      override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data)      override;

                bool VBindTexture(std::string name, ITexture* texture)      override;
                bool VUnbindTexture(std::string name, ITexture* texture)    override;

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

                ///Prepare this Material by building a descriptor set based off of its shader variables
                bool VPrepare()                                                 override;

            protected:
                VKRenderPass* m_renderPass;

                VkPipelineRasterizationStateCreateInfo m_rasterizationState;
                VkPipelineMultisampleStateCreateInfo m_multisampleState;
                std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

                VkDescriptorSetLayout   m_descriptorLayout;
                VkPipelineLayout        m_pipelineLayout;

                VkDescriptorSet m_descriptorSet; //Collection of shader variables
                VkPipelineCache m_pipelineCache;
                VkPipeline      m_pipeline;

            private:
                bool PrepareLayouts(VkDevice device);
                bool PrepareDescriptorSet(VkDevice device);
                bool PreparePipeline(VkDevice device);
            };
        }
    }
}
