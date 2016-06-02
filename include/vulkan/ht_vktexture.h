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

#pragma once

#include <ht_texture.h>
#include <ht_texture_base.h>
#include <ht_texture_resource.h>
#include <ht_vksampler.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {
    
        namespace Vulkan {

            class HT_API VKTexture : public TextureBase
            {
            public:
                VKTexture();
                virtual ~VKTexture();

                //Required function for RefCounted classes
                bool Initialize(Resource::TextureHandle handle, const VkDevice& device);

                //For building a texture *not* from a file
                bool Initialize(const VkDevice& device, const BYTE* data, uint32_t width, uint32_t height, uint32_t channelCount, uint32_t mipLevels);

                VkImageView GetView();

            private:
                bool VKBufferImage();

                VkDevice m_device;

                VkImageView m_view;
                VkImage m_image;
                VkImageLayout m_imageLayout;

                VkDeviceMemory m_deviceMemory;
            };

        }
    }
}