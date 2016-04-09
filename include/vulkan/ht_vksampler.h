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
#include <ht_sampler_resource.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {
        
            class HT_API VKSampler : public Core::RefCounted<VKSampler>, public ISampler
            {
            public:
                VKSampler(std::string ID);
                ~VKSampler();

                //Required function of RefCounted classes
                bool Initialize(const std::string& fileName);

                bool VDeferredInitialize(const std::string& fileName) override;

                //bool VPrepare() override;

                VkSampler GetVkSampler();
                VkFormat  GetVkColorSpace();

            private:
                const VkDevice&             m_device;
                VkSampler                   m_sampler;
                VkFormat                    m_colorSpace;
                std::string                 m_fileName;
                Hatchit::Resource::Sampler  m_base;


                VkSamplerAddressMode    VKAddressModeFromType(Resource::Sampler::AddressMode mode);
                VkFilter                VKFilterModeFromType(Resource::Sampler::FilterMode mode);
                VkFormat                VKColorSpaceFromType(Resource::Sampler::ColorSpace space);
                VkCompareOp             VKCompareOpFromType(Resource::Sampler::CompareOperation op);
                VkSamplerMipmapMode     VKMipMapModeFromType(Resource::Sampler::MipMode mode);
                VkBorderColor           VKBorderColorFromType(Resource::Sampler::BorderColor color);
            };

			using VKSamplerHandle = Core::Handle<VKSampler>;
        }

    }

}
