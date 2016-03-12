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

#include <ht_sampler.h>

namespace Hatchit {

    namespace Graphics {

        ISampler::ISampler() 
        {
            m_filterMode = FilterMode::BILINEAR;
            m_wrapMode = WrapMode::CLAMP;
            m_colorSpace = ColorSpace::LINEAR;
        }

        bool ISampler::VInitFromFile(Core::File* file)
        {
            //TODO: Parse JSON file

            return VPrepare();
        }

        void ISampler::SetFilterMode(FilterMode filterMode) { m_filterMode = filterMode; }
        void ISampler::SetWrapMode(WrapMode wrapMode) { m_wrapMode = wrapMode; }
        void ISampler::SetColorSpace(ColorSpace colorSpace) { m_colorSpace = colorSpace; }

        FilterMode ISampler::GetFilterMode(){ return m_filterMode; }
        WrapMode ISampler::GetWrapMode() { return m_wrapMode; }
        ColorSpace ISampler::GetColorSpace(){ return m_colorSpace; }

    }

}