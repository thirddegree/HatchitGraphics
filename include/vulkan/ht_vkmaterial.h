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
#include <ht_vkpipeline.h>
#include <ht_vktexture.h>
#include <ht_material_resource.h>
#include <ht_refcounted.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKMaterial : public Core::RefCounted<VKMaterial>, public IMaterial
            {
            public:
                VKMaterial(std::string ID);
                ~VKMaterial();

                //Required function for RefCounted class
                bool Initialize(const std::string& fileName);

                bool VDeferredInitialize(Resource::MaterialHandle resource) override;

                bool VSetInt(std::string name, int data)                    override;
                bool VSetFloat(std::string name, float data)                override;
                bool VSetFloat3(std::string name, Math::Vector3 data)       override;
                bool VSetFloat4(std::string name, Math::Vector4 data)       override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data)      override;

                bool VBindTexture(std::string name, ITextureHandle texture)      override;
                bool VUnbindTexture(std::string name, ITextureHandle texture)    override;

                bool VUpdate()                                              override;

                VkDescriptorSet* GetVKDescriptorSet();

                IPipelineHandle GetPipeline() override;

            private:
                const VkDevice& m_device;

                bool setupDescriptorSet(VkDescriptorPool descriptorPool);

                Resource::MaterialHandle m_materialResourceHandle;

                VkDescriptorSetLayout m_materialLayout;
                VkDescriptorSet m_materialSet;

                UniformBlock m_uniformVSBuffer;
                //UniformBlock m_uniformFSBuffer;
                std::vector<UniformBlock> m_fragmentTextures;

                VKPipelineHandle m_pipeline;
                std::map<std::string, ITextureHandle> m_textures;
                std::map<std::string, Hatchit::Resource::ShaderVariable*> m_shaderVariables;
            };

            using VKMaterialHandle = Core::Handle<VKMaterial>;
        }
    }
}