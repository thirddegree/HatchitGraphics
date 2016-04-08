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

#include <ht_vkrenderer.h>
#include <ht_vksampler.h>
#include <ht_debug.h>
#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKSampler::VKSampler(std::string ID, const std::string& fileName) :
                RefCounted<VKSampler>(std::move(ID)),
                m_device(VKRenderer::RendererInstance->GetVKDevice())
            {
                Resource::SamplerHandle handle = Resource::MutableSampler::GetHandleFromFileName(fileName);

                if (handle.IsValid())
                {
                    
                }
            }
            VKSampler::~VKSampler() 
            {
                vkDestroySampler(m_device, m_sampler, nullptr);
            }

            bool VKSampler::VPrepare() 
            {
                Resource::SamplerHandle handle = Resource::MutableSampler::GetHandleFromFileName(m_fileName);

                if (!handle.IsValid())
                {
                    HT_DEBUG_PRINTF("Failed to retrieve handle for VKSampler prepar()\n");
                    return false;
                }

                VkResult err;

                //Setup the sampler
                VkSamplerCreateInfo samplerInfo = {};
                samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.pNext = nullptr;
                samplerInfo.magFilter = VKFilterModeFromType(handle->GetFilter().mag);
                samplerInfo.minFilter = VKFilterModeFromType(handle->GetFilter().min);
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.addressModeU = VKAddressModeFromType(handle->GetAddress().u);
                samplerInfo.addressModeV = VKAddressModeFromType(handle->GetAddress().v);
                samplerInfo.addressModeW = VKAddressModeFromType(handle->GetAddress().w);
                samplerInfo.mipLodBias = 0.0f;
                samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
                samplerInfo.minLod = 0.0f;
                samplerInfo.maxLod = 0.0f;
                samplerInfo.maxAnisotropy = 8;
                samplerInfo.anisotropyEnable = VK_TRUE;
                samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

                err = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler);
                assert(!err);
                if (err != VK_SUCCESS)
                {
                    HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to create sampler!\n");
                    return false;
                }

                return true;
            }

            VkSampler VKSampler::GetVkSampler() { return m_sampler; }

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

        }

    }

}
