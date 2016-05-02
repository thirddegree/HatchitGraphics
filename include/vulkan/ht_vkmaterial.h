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

                bool VSetInt(std::string name, int data)                    override;
                bool VSetFloat(std::string name, float data)                override;
                bool VSetFloat3(std::string name, Math::Vector3 data)       override;
                bool VSetFloat4(std::string name, Math::Vector4 data)       override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data)      override;

                bool VBindTexture(std::string name, TextureHandle texture)      override;
                bool VUnbindTexture(std::string name, TextureHandle texture)    override;

                bool VUpdate()                                              override;

                const void BindMaterial(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout) const;

            private:
                const VkDevice* m_device;
                const VkDescriptorPool* m_descriptorPool;

                bool setupDescriptorSet();

                std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
                std::vector<VkDescriptorSetLayout> m_materialLayouts;
                std::vector<VkDescriptorSet> m_materialSets;

                UniformBlock_vk m_uniformVSBuffer;
                //UniformBlock m_uniformFSBuffer;
                std::vector<UniformBlock_vk> m_fragmentTextures;
                
                std::vector<LayoutLocation> m_textureLocations;
                std::vector<TextureHandle> m_textures;

                //std::vector<LayoutLocation> m_shaderVariableLocations;
                //std::vector<std::map<std::string, Hatchit::Resource::ShaderVariable*>> m_shaderVariables;
            };
        }
    }
}