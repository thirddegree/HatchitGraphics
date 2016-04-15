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
#include <ht_directx.h>
#include <ht_texture.h>
#include <ht_image.h>
#include <ht_refcounted.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12Texture : public Core::RefCounted<D3D12Texture>, public Texture
            {
            public:
                D3D12Texture(std::string ID);

                ~D3D12Texture();

                //void Upload(ID3D12GraphicsCommandList* commandList);

                bool Initialize(const std::string& fileName, ID3D12Device* device, ID3D12DescriptorHeap* heap);

                void SetSampler(ISamplerHandle sampler) override;

            private:
                D3D12_RESOURCE_DESC         m_desc;
                ID3D12Resource*             m_texture;
                ID3D12Resource*             m_uploadHeap;
                Resource::Image*            m_bitmap;
            };
        }
    }
}