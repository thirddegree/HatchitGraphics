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

                auto device = resources->GetDevice();

                m_handle = Resource::Texture::GetHandleFromFileName(fileName);
                if (!m_handle.IsValid())
                    return false;


                /*Descibe and create a Texture2D*/
                m_desc = {};
                m_desc.MipLevels = 1;
                m_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                m_desc.Width = m_handle->GetWidth();
                m_desc.Height = m_handle->GetHeight();
                m_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
                m_desc.DepthOrArraySize = 1;
                m_desc.SampleDesc.Count = 1;
                m_desc.SampleDesc.Quality = 0;
                m_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

                HRESULT hr = S_OK;

                hr = device->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &m_desc,
                    D3D12_RESOURCE_STATE_COPY_DEST,
                    nullptr,
                    IID_PPV_ARGS(&m_texture));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("ID3D12Texture::VInitFromFile, Failed to create texture.\n");
                    return false;
                }

                /*Create upload heap*/
                const uint64_t uploadSize = GetRequiredIntermediateSize(m_texture.Get(), 0, 1);
                hr = device->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(uploadSize),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_uploadHeap));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("ID3D12Texture::VInitFromFile, Failed to create texture upload heap.\n");
                    return false;
                }




                return true;
            }

            void D3D12Texture::Upload(D3D12DeviceResources* resources, uint32_t descriptorOffset)
            {
                D3D12_SUBRESOURCE_DATA data = {};
                data.pData = m_handle->GetData();
                data.RowPitch = m_handle->GetWidth() * 4;
                data.SlicePitch = m_handle->GetMIPLevels();

                auto cmdList = resources->GetCommandList();

                cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(),
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

                // Use Heap-allocating UpdateSubresources implementation for variable number of subresources (which is the case for textures).
                UpdateSubresources(cmdList, m_texture.Get(), m_uploadHeap.Get(), 0, 0, 1, &data);

                cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(),
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

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
