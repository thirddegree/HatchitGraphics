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

#include <ht_vksampler.h>
#include <ht_debug.h>
#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKSampler::VKSampler()
            {}

            bool VKSampler::InitFromResource(const Resource::Sampler& sampler, const VkDevice& device)
            {
                m_device = device;
                m_sampler = nullptr;
                VkResult err;

                Resource::Sampler::Filter filter = sampler.GetFilter();
                Resource::Sampler::Address address = sampler.GetAddress();

                //Setup the sampler
                VkSamplerCreateInfo samplerInfo = {};
                samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.pNext = nullptr;
                samplerInfo.magFilter = VKFilterModeFromType(filter.mag);
                samplerInfo.minFilter = VKFilterModeFromType(filter.min);
                samplerInfo.mipmapMode = VKMipMapModeFromType(sampler.GetMipMode());
                samplerInfo.addressModeU = VKAddressModeFromType(address.u);
                samplerInfo.addressModeV = VKAddressModeFromType(address.v);
                samplerInfo.addressModeW = VKAddressModeFromType(address.w);
                samplerInfo.mipLodBias = sampler.GetMipLODBias();
                samplerInfo.compareOp = VKCompareOpFromType(sampler.GetCompareOp());
                samplerInfo.minLod = sampler.GetMinLOD();
                samplerInfo.maxLod = sampler.GetMaxLOD();
                samplerInfo.maxAnisotropy = static_cast<float>(sampler.GetMaxAnisotropy());
                samplerInfo.anisotropyEnable = VK_TRUE;
                samplerInfo.borderColor = VKBorderColorFromType(sampler.GetBorderColor());

                err = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to create sampler!\n");
                    return false;
                }

                return true;
            }

            VKSampler::~VKSampler() 
            {
                vkDestroySampler(m_device, m_sampler, nullptr);
            }

            VkSampler VKSampler::GetVkSampler()
            { 
                return m_sampler; 
            }

            VkSamplerAddressMode VKSampler::VKAddressModeFromType(Resource::Sampler::AddressMode mode)
            {
                using namespace Resource;

                switch (mode)
                {
                case Sampler::AddressMode::CLAMP:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                case Sampler::AddressMode::WRAP:
                    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
                case Sampler::AddressMode::BORDER:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                case Sampler::AddressMode::MIRROR:
                    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                case Sampler::AddressMode::MIRROR_ONCE:
                    return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
                }

                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            }

            VkFilter VKSampler::VKFilterModeFromType(Resource::Sampler::FilterMode mode)
            {
                using namespace Resource;
                
                switch (mode)
                {
                case Sampler::FilterMode::BILINEAR:
                    return VK_FILTER_LINEAR;
                case Sampler::FilterMode::NEAREST:
                    return VK_FILTER_NEAREST;
                default:
                    break;
                }

                return VK_FILTER_LINEAR;
            }

            VkFormat VKSampler::VKColorSpaceFromType(Resource::Sampler::ColorSpace space)
            {
                using namespace Resource;

                switch (space)
                {
                case Sampler::ColorSpace::GAMMA:
                    return VK_FORMAT_R8G8B8A8_SRGB;
                case Sampler::ColorSpace::LINEAR:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                default:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                }

            }


            VkCompareOp VKSampler::VKCompareOpFromType(Resource::Sampler::CompareOperation op)
            {
                using namespace Resource;

                switch (op)
                {
                case Sampler::CompareOperation::COMPARE_OP_ALWAYS:
                    return VK_COMPARE_OP_ALWAYS;
                case Sampler::CompareOperation::COMPARE_OP_EQUAL:
                    return VK_COMPARE_OP_EQUAL;
                case Sampler::CompareOperation::COMPARE_OP_GREATER:
                    return VK_COMPARE_OP_GREATER;
                case Sampler::CompareOperation::COMPARE_OP_GREATER_EQUAL:
                    return VK_COMPARE_OP_GREATER_OR_EQUAL;
                case Sampler::CompareOperation::COMPARE_OP_LESS:
                    return VK_COMPARE_OP_LESS;
                case Sampler::CompareOperation::COMPARE_OP_LESS_EQUAL:
                    return VK_COMPARE_OP_LESS_OR_EQUAL;
                case Sampler::CompareOperation::COMPARE_OP_NEVER:
                    return VK_COMPARE_OP_NEVER;
                case Sampler::CompareOperation::COMPARE_OP_NOT_EQUAL:
                    return VK_COMPARE_OP_NOT_EQUAL;
                default:
                    return VK_COMPARE_OP_NEVER;
                }
            }

            VkSamplerMipmapMode VKSampler::VKMipMapModeFromType(Resource::Sampler::MipMode mode)
            {
                using namespace Resource;

                switch (mode)
                {
                case Sampler::MipMode::LINEAR:
                    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
                case Sampler::MipMode::NEAREST:
                    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
                default:
                    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
                }

            }

            VkBorderColor VKSampler::VKBorderColorFromType(Resource::Sampler::BorderColor color)
            {
                using namespace Resource;

                switch (color)
                {
                case Sampler::BorderColor::COLOR_OPAQUE_BLACK:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
                case Sampler::BorderColor::COLOR_OPAQUE_WHITE:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                case Sampler::BorderColor::COLOR_TRANSPARENT_BLACK:
                    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
                default:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                }
            }

        }

    }

}
