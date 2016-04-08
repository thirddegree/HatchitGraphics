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

            VKSampler::VKSampler(std::string ID) :
                RefCounted<VKSampler>(std::move(ID)),
                m_device(VKRenderer::RendererInstance->GetVKDevice())
            {}

            bool VKSampler::Initialize(const std::string& fileName)
            {
                Resource::SamplerHandle handle = Resource::Sampler::GetHandleFromFileName(fileName);

                if (handle.IsValid())
                {
                    m_filterMode = handle->GetFilterMode();
                    m_wrapMode = handle->GetWrapMode();
                    m_colorSpace = handle->GetColorSpace();
                    return true;
                }
                else
                {
                    HT_DEBUG_PRINTF("Unable to initialize VKSampler: Missing Resource from file %s", fileName);
                    return false;
                }
            }
            VKSampler::~VKSampler() 
            {
                vkDestroySampler(m_device, m_sampler, nullptr);
            }

            bool VKSampler::VPrepare() 
            {
                VkResult err;

                //Determine some sampler settings
                VkSamplerAddressMode vkWrapMode = {};
                VkFilter vkFilterMode = {};

                switch (m_wrapMode)
                {
                case Resource::Sampler::WrapMode::MIRROR:
                    vkWrapMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                    break;
                case Resource::Sampler::WrapMode::MIRROR_ONCE:
                    vkWrapMode = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
                    break;
                case Resource::Sampler::WrapMode::BORDER:
                    vkWrapMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                    break;
                case Resource::Sampler::WrapMode::CLAMP:
                    vkWrapMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                    break;
                case Resource::Sampler::WrapMode::WRAP:
                    vkWrapMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    break;
                default:
                    vkWrapMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    break;
                }

                switch (m_filterMode)
                {
                case Resource::Sampler::FilterMode::NEAREST:
                    vkFilterMode = VK_FILTER_NEAREST;
                    break;
                case Resource::Sampler::FilterMode::BILINEAR:
                    vkFilterMode = VK_FILTER_LINEAR;
                    break;
                default:
                    vkFilterMode = VK_FILTER_LINEAR;
                    break;
                }

                //Setup the sampler
                VkSamplerCreateInfo samplerInfo = {};
                samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.pNext = nullptr;
                samplerInfo.magFilter = vkFilterMode;
                samplerInfo.minFilter = vkFilterMode;
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.addressModeU = vkWrapMode;
                samplerInfo.addressModeV = vkWrapMode;
                samplerInfo.addressModeW = vkWrapMode;
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

            void VKSampler::SetFilterMode(Resource::Sampler::FilterMode filterMode)
            {
                m_filterMode = filterMode;
            }

            void VKSampler::SetWrapMode(Resource::Sampler::WrapMode wrapMode)
            {
                m_wrapMode = wrapMode;
            }

            void VKSampler::SetColorSpace(Resource::Sampler::ColorSpace colorSpace)
            {
                m_colorSpace = colorSpace;
            }

            Resource::Sampler::FilterMode VKSampler::GetFilterMode() const
            {
                return m_filterMode;
            }

            Resource::Sampler::WrapMode VKSampler::GetWrapMode() const
            {
                return m_wrapMode;
            }

            Resource::Sampler::ColorSpace VKSampler::GetColorSpace() const
            {
                return m_colorSpace;
            }

        }

    }

}
