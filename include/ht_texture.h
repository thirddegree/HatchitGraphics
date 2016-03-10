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

#include <ht_platform.h>
#include <ht_resourceobject.h>

namespace Hatchit {

    namespace Graphics {
        
        enum WrapMode 
        {
            REPEAT,
            CLAMP
        };

        enum FilterMode
        {
            POINT,
            BILINEAR,
            TRILINEAR
        };

        enum ColorSpace 
        {
            GAMMA,
            LINEAR
        };

        class HT_API ITexture : public Resource::ResourceObject
        {
        public:
            virtual ~ITexture() { }
        
            virtual size_t VGetWidth()  const = 0;
            virtual size_t VGetHeight() const = 0;

        protected:
            uint32_t m_width, m_height;

            FilterMode m_filterMode;
            WrapMode m_wrapMode;
            ColorSpace m_colorSpace;
        };
    }
}
