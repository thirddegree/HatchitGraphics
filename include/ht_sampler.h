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
            NEAREST,
            BILINEAR
        };

        enum ColorSpace
        {
            GAMMA,
            LINEAR
        };

        class HT_API ISampler : public Resource::ResourceObject
        {
        public:
            ISampler();
            virtual ~ISampler() {};

            bool VInitFromFile(Core::File* file) override;

            virtual bool VPrepare() = 0;

            void SetFilterMode(FilterMode filterMode);
            void SetWrapMode(WrapMode wrapMode);
            void SetColorSpace(ColorSpace colorSpace);

            FilterMode GetFilterMode();
            WrapMode GetWrapMode();
            ColorSpace GetColorSpace();

        protected:
            FilterMode m_filterMode;
            WrapMode m_wrapMode;
            ColorSpace m_colorSpace;
        };

    }

}