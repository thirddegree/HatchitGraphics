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

#pragma once

#include <ht_texture.h>
#include <ht_texture_resource.h>
#include <ht_vksampler.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {
    
        namespace Vulkan {

            class HT_API VKTexture : public Core::RefCounted<VKTexture>, public ITexture
            {
            public:
                VKTexture(std::string ID);
                virtual ~VKTexture();

                //Required function for RefCounted classes
                bool Initialize(const std::string& fileName);

                VkSampler GetSampler();
                VkImageView GetView();

                virtual void SetSampler(ISamplerHandle sampler) override;

                virtual uint32_t GetWidth() const override;
                virtual uint32_t GetHeight() const override;

            private:
                bool VBufferImage() override;

                VkDevice m_device;

                VkImageView m_view;
                VkImage m_image;
                VkImageLayout m_imageLayout;

                VkDeviceMemory m_deviceMemory;

                Resource::TextureHandle m_resource;

                VKSamplerHandle m_sampler;
            };

            using VKTextureHandle = Core::Handle<VKTexture>;

        }
    }
}