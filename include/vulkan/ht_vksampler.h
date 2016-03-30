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

#include <ht_sampler.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {
        
            class HT_API VKSampler : public ISampler 
            {
            public:
                VKSampler(VkDevice device, const std::string& samplerResourceFile);
                ~VKSampler();

                bool VPrepare() override;

                VkSampler GetVkSampler();

                // Inherited via ISampler
                virtual void SetFilterMode(Resource::Sampler::FilterMode filterMode) override;
                virtual void SetWrapMode(Resource::Sampler::WrapMode wrapMode) override;
                virtual void SetColorSpace(Resource::Sampler::ColorSpace colorSpace) override;
                virtual Resource::Sampler::FilterMode GetFilterMode() override;
                virtual Resource::Sampler::WrapMode GetWrapMode() override;
                virtual Resource::Sampler::ColorSpace GetColorSpace() override;

            private:
                VkDevice m_device;

                VkSampler m_sampler;

                Resource::SamplerHandle m_resources;
            };

        }

    }

}
