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

#include <ht_d3d12texture.h>
#include <ht_image.h>
#include <ht_debug.h>
#include <ht_texture_resource.h>
#include <ht_d3d12deviceresources.h>

#include <DDSTextureLoader.h>
#include <ht_path_singleton.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12Texture::D3D12Texture(std::string ID)
                : Core::RefCounted<D3D12Texture>(std::move(ID))
            {
                
                
            }

            D3D12Texture::~D3D12Texture()
            {
                
                
            }


            bool D3D12Texture::Initialize(const std::string & fileName, D3D12DeviceResources* resources)
            {
                using namespace Resource;

                HRESULT hr = DirectX::CreateDDSTextureFromFile12(resources->GetDevice(),
                    resources->GetCommandList(), L"raptor.dds",
                    m_texture,
                    m_uploadHeap);
                if (FAILED(hr))
                    return false;

                return true;
            }

            void D3D12Texture::Upload(D3D12DeviceResources* resources, uint32_t descriptorOffset)
            {
                // Describe and create a SRV for the texture.
                CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(resources->GetRootLayout()->GetHeap(D3D12RootLayout::HeapType::CBV_SRV_UAV)->GetCPUDescriptorHandleForHeapStart());
                cpuHandle.Offset(descriptorOffset, resources->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = m_texture->GetDesc().Format;
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = 1;
                resources->GetDevice()->CreateShaderResourceView(m_texture.Get(),
                    &srvDesc, cpuHandle);
            }


            HRESULT D3D12Texture::CreateD3DResourceFromHandle(const Resource::TextureHandle& handle)
            {
                if (!handle.IsValid())
                    return E_FAIL;

                std::unique_ptr<uint8_t[]> imageData;


                return S_OK;
            }
            
        }
    }
}
