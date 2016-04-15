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
#include <cassert>

namespace Hatchit {

    namespace Graphics {

        class HT_API Texture
        {
        public:
            virtual ~Texture() = default;
        
            bool Initialize(const std::string& fileName);

            virtual size_t GetWidth()  const;
            virtual size_t GetHeight() const;

        protected:
            size_t m_width;
            size_t m_height;
            uint32_t m_bitsPerPixel;
            uint32_t m_channelCount;
            uint32_t m_mipLevels;

            const BYTE* m_data;
        };

        using TextureHandle = Core::Handle<Texture>;
    }
}
