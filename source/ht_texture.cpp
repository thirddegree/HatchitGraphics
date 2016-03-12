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

#include <ht_texture.h>
#include <ht_image.h>

namespace Hatchit {

    namespace Graphics {

        bool ITexture::VInitFromFile(Core::File* file)
        {
            Resource::Image* bitmap = Resource::Image::Load(file, Resource::Image::Channels::AUTO);

            m_width = bitmap->GetWidth();
            m_height = bitmap->GetHeight();
            m_mipLevels = 1;

            m_bpp = static_cast<uint8_t>(bitmap->GetBPP());
            m_channels = static_cast<uint32_t>(bitmap->GetChannels());

            //The size of this should be width * height * channels
            m_data = bitmap->GetData();

            return VBufferImage();
        }

        void ITexture::SetSampler(ISampler* sampler) { m_sampler = sampler; }

        uint32_t ITexture::GetWidth() const { return m_width; }
        uint32_t ITexture::GetHeight() const { return m_height; }

    }

}