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

            class HT_API VKMaterial : public IMaterial
            {
            public:
                VKMaterial();
                ~VKMaterial();

                ///Callback for when this VKMaterial has been loaded from the disk. Used to build shader variable list
                bool VInitFromFile(Core::File* file)                        override;

                bool VSetInt(std::string name, int data)                    override;
                bool VSetFloat(std::string name, float data)                override;
                bool VSetFloat3(std::string name, Math::Vector3 data)       override;
                bool VSetFloat4(std::string name, Math::Vector4 data)       override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data)      override;

                bool VBindTexture(std::string name, ITexture* texture)      override;
                bool VUnbindTexture(std::string name, ITexture* texture)    override;

                ///Prepare this Material by building a descriptor set based off of its shader variables
                bool VPrepare(IPipeline* pipeline)                         override;
                bool VUpdate()                                              override;

                VkDescriptorSet* GetVKDescriptorSet();

            private:
                VkDescriptorSetLayout m_materialLayout;
                VkDescriptorSet m_materialSet;

                UniformBlock m_uniformVSBuffer;
                UniformBlock m_uniformFSBuffer;
                std::vector<UniformBlock> m_fragmentTextures;

                bool setupDescriptorSet(VkDescriptorPool descriptorPool, VkDevice device);
            };
        }
    }
}