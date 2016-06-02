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

#include <ht_platform.h>
#include <ht_types.h>

namespace Hatchit
{
    namespace Graphics
    {
        class HT_API TextureBase
        {
        public:
            virtual ~TextureBase() = default;

            uint32_t GetWidth() const;
            uint32_t GetHeight() const;

        protected:
            const BYTE* m_data;
            uint32_t    m_width;
            uint32_t    m_height;
            uint32_t    m_bpp;
            uint32_t    m_channels;
            uint32_t    m_mipLevels;

            friend class Texture;
        };
    }
}