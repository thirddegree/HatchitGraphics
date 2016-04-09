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

namespace Hatchit
{
    namespace Core
    {
        template<typename VarType>
        class Handle;
    }

    namespace Resource
    {
        class Texture;
    }

    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12Texture : public ITexture
            {
            public:
                D3D12Texture(ID3D12Device* device);

                ~D3D12Texture();

                bool VDeferredInitialize(Core::Handle<const Resource::Texture> resource) override;

                //void Upload(ID3D12GraphicsCommandList* commandList);

                void SetSampler(ISamplerHandle sampler) override;

                uint32_t GetWidth() const override;
                uint32_t GetHeight() const override;

            private:
                D3D12_RESOURCE_DESC         m_desc;
                ID3D12Device*               m_device;
                ID3D12Resource*             m_texture;
                ID3D12Resource*             m_uploadHeap;
                ID3D12DescriptorHeap*       m_srvHeap;
                Resource::Image*            m_bitmap;

                // Inherited via ITexture
                virtual bool VBufferImage() override;
            };
        }
    }
}