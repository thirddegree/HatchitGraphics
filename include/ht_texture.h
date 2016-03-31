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

#include <ht_debug.h>
#include <ht_platform.h>
#include <ht_sampler.h>
#include <ht_resourceobject.h>
#include <cassert>

namespace Hatchit {

    namespace Graphics {

        class HT_API ITexture : public Resource::ResourceObject
        {
        public:
            virtual ~ITexture() { }
        
            void SetSampler(ISampler* sampler);

            bool VInitFromFile(Core::File* file) override;

            uint32_t GetWidth()  const;
            uint32_t GetHeight() const;

        protected:
            virtual bool VBufferImage() = 0;

            uint32_t m_width, m_height;
            uint32_t m_mipLevels;
            uint8_t m_bpp, m_channels;

            const BYTE* m_data;

            ISampler* m_sampler;
        };
    }
}
