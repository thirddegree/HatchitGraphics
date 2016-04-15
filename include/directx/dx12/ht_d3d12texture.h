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
#include <ht_texture_resource.h>
#include <ht_image.h>
#include <ht_refcounted.h>
#include <wrl/client.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class D3D12DeviceResources;

            class HT_API D3D12Texture : public Core::RefCounted<D3D12Texture>, public ITexture
            {
            public:
                D3D12Texture(std::string ID);

                ~D3D12Texture();

                void Upload(D3D12DeviceResources* resources, ID3D12GraphicsCommandList* commandList);

                bool Initialize(const std::string& fileName, D3D12DeviceResources* resources, ID3D12GraphicsCommandList* commandList);

                void SetSampler(ISamplerHandle sampler) override;

                uint32_t GetWidth() const override;
                uint32_t GetHeight() const override;

            private:
                D3D12_RESOURCE_DESC         m_desc;
                Microsoft::WRL::ComPtr<ID3D12Resource>             m_texture;
                Microsoft::WRL::ComPtr<ID3D12Resource>             m_uploadHeap;
                Resource::TextureHandle     m_handle;
                uint32_t                    m_width;
                uint32_t                    m_height;

                // Inherited via ITexture
                virtual bool VBufferImage() override;


                static HRESULT CreateD3DResourceFromHandle(const Resource::TextureHandle& handle);
            };

            using D3D12TextureHandle = Core::Handle<D3D12Texture>;
        }
    }
}