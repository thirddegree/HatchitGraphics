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

#include <ht_samplerresource.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API ISampler
        {
        public:
            virtual ~ISampler() = default;

            virtual bool VPrepare() = 0;

            virtual void SetFilterMode(Resource::Sampler::FilterMode filterMode) = 0;
            virtual void SetWrapMode(Resource::Sampler::WrapMode wrapMode) = 0;
            virtual void SetColorSpace(Resource::Sampler::ColorSpace colorSpace) = 0;
            
            virtual Resource::Sampler::FilterMode GetFilterMode() = 0;
            virtual Resource::Sampler::WrapMode GetWrapMode() = 0;
            virtual Resource::Sampler::ColorSpace GetColorSpace() = 0;
        };

    }

}