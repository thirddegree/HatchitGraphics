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
#include <ht_material_base.h>
#include <ht_vkrenderpass.h>
#include <ht_vkpipeline.h>
#include <ht_vktexture.h>
#include <ht_material_resource.h>
#include <ht_refcounted.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKMaterial :  public MaterialBase
            {
            public:
                VKMaterial();
                ~VKMaterial();

                //Required function for RefCounted class
                bool Initialize(Resource::MaterialHandle handle, const VkDevice& device, const VkDescriptorPool& descriptorPool);

                bool VBindTexture(std::string name, TextureHandle texture)      override;
                bool VUnbindTexture(std::string name, TextureHandle texture)    override;

                bool VUpdate()                                              override;

                const void BindMaterial(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout) const;
                
                PipelineHandle const VGetPipeline() const override;
                const VKPipeline* GetVKPipeline() const;

            private:
                const VkDevice* m_device;
                const VkDescriptorPool* m_descriptorPool;

                bool setupDescriptorSet();

                PipelineHandle m_pipelineHandle;
                VKPipeline* m_pipeline;

                std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
                std::vector<VkDescriptorSetLayout> m_materialLayouts;
                std::vector<VkDescriptorSet> m_materialSets;

                std::map<std::string, Graphics::TextureHandle> m_textureHandles;

                UniformBlock_vk m_uniformVSBuffer;
                //UniformBlock m_uniformFSBuffer;
                std::vector<UniformBlock_vk> m_fragmentTextures;
                
                std::vector<LayoutLocation> m_textureLocations;
                std::vector<VKTexture*> m_textures;
            };
        }
    }
}