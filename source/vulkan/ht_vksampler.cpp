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
                /*Resource::SamplerHandle handle = Resource::Sampler::GetHandleFromFileName(fileName);
                m_sampler = nullptr;
                    return true;
                }
                else
                {
                    HT_DEBUG_PRINTF("Unable to initialize VKSampler: Missing Resource from file %s", fileName);
                    return false;
                }*/
                return true;
            }

            bool VKSampler::VDeferredInitialize(const std::string& fileName)
            {
                m_fileName = fileName;
                m_sampler = nullptr;

                //Contents of old VPrepare
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
                samplerInfo.mipmapMode = VKMipMapModeFromType(handle->GetMipMode());
                samplerInfo.addressModeU = VKAddressModeFromType(handle->GetAddress().u);
                samplerInfo.addressModeV = VKAddressModeFromType(handle->GetAddress().v);
                samplerInfo.addressModeW = VKAddressModeFromType(handle->GetAddress().w);
                samplerInfo.mipLodBias = handle->GetMipLODBias();
                samplerInfo.compareOp = VKCompareOpFromType(handle->GetCompareOp());
                samplerInfo.minLod = handle->GetMinLOD();
                samplerInfo.maxLod = handle->GetMaxLOD();
                samplerInfo.maxAnisotropy = static_cast<float>(handle->GetMaxAnisotropy());
                samplerInfo.anisotropyEnable = VK_TRUE;
                samplerInfo.borderColor = VKBorderColorFromType(handle->GetBorderColor());
                m_colorSpace = VKColorSpaceFromType(handle->GetColorSpace());

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

            //bool VKSampler::VPrepare() 
            //{
            //    Resource::SamplerHandle handle = Resource::MutableSampler::GetHandleFromFileName(m_fileName);

            //    if (!handle.IsValid())
            //    {
            //        HT_DEBUG_PRINTF("Failed to retrieve handle for VKSampler prepar()\n");
            //        return false;
            //    }

            //    VkResult err;

            //    //Setup the sampler
            //    VkSamplerCreateInfo samplerInfo = {};
            //    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            //    samplerInfo.pNext = nullptr;
            //    samplerInfo.magFilter = VKFilterModeFromType(handle->GetFilter().mag);
            //    samplerInfo.minFilter = VKFilterModeFromType(handle->GetFilter().min);
            //    samplerInfo.mipmapMode = VKMipMapModeFromType(handle->GetMipMode());
            //    samplerInfo.addressModeU = VKAddressModeFromType(handle->GetAddress().u);
            //    samplerInfo.addressModeV = VKAddressModeFromType(handle->GetAddress().v);
            //    samplerInfo.addressModeW = VKAddressModeFromType(handle->GetAddress().w);
            //    samplerInfo.mipLodBias = handle->GetMipLODBias();
            //    samplerInfo.compareOp = VKCompareOpFromType(handle->GetCompareOp());
            //    samplerInfo.minLod = handle->GetMinLOD();
            //    samplerInfo.maxLod = handle->GetMaxLOD();
            //    samplerInfo.maxAnisotropy = static_cast<float>(handle->GetMaxAnisotropy());
            //    samplerInfo.anisotropyEnable = VK_TRUE;
            //    samplerInfo.borderColor = VKBorderColorFromType(handle->GetBorderColor());
            //    m_colorSpace = VKColorSpaceFromType(handle->GetColorSpace());

            //    err = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler);
            //    assert(!err);
            //    if (err != VK_SUCCESS)
            //    {
            //        HT_DEBUG_PRINTF("VKTexture::VBufferImage(): Failed to create sampler!\n");
            //        return false;
            //    }

            //    return true;
            //}

            VkSampler VKSampler::GetVkSampler()
            { 
                return m_sampler; 
            }

            VkFormat VKSampler::GetVkColorSpace()
            {
                return m_colorSpace;
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
