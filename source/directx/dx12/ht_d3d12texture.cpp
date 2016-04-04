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

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12Texture::D3D12Texture(ID3D12Device* device)
            {
                m_device = device;
                m_bitmap = nullptr;
                m_texture = nullptr;
                m_uploadHeap = nullptr;
            }

            D3D12Texture::~D3D12Texture()
            {
                ReleaseCOM(m_texture);
                ReleaseCOM(m_uploadHeap);
                
                delete m_bitmap;
            }

            uint32_t D3D12Texture::GetHeight() const
            {
                return 0;
            }

            uint32_t D3D12Texture::GetWidth() const
            {
                return 0;
            }

            void D3D12Texture::SetSampler(ISampler* sampler)
            {

            }

            bool D3D12Texture::VBufferImage()
            {
                return true;
            }

//            bool D3D12Texture::VInitFromFile(Core::File* file)
//            {
//                m_bitmap = Resource::Image::Load(file, Resource::Image::Channels::AUTO);
//                if (!m_bitmap)
//                    return false;
//
//                /*Descibe and create a Texture2D*/
//                m_desc = {};
//                m_desc.MipLevels = 1;
//                m_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//                m_desc.Width = m_bitmap->GetWidth();
//                m_desc.Height = m_bitmap->GetHeight();
//                m_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
//                m_desc.DepthOrArraySize = 1;
//                m_desc.SampleDesc.Count = 1;
//                m_desc.SampleDesc.Quality = 0;
//                m_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//
//                HRESULT hr = S_OK;
//
//                hr = m_device->CreateCommittedResource(
//                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//                    D3D12_HEAP_FLAG_NONE,
//                    &m_desc,
//                    D3D12_RESOURCE_STATE_COPY_DEST,
//                    nullptr,
//                    IID_PPV_ARGS(&m_texture));
//                if (FAILED(hr))
//                {
//#ifdef _DEBUG
//                    Core::DebugPrintF("ID3D12Texture::VInitFromFile, Failed to create texture.\n");
//#endif
//                    return false;
//                }
//
//                /*Create upload heap*/
//                const uint64_t uploadSize = GetRequiredIntermediateSize(m_texture, 0, 1);
//                hr = m_device->CreateCommittedResource(
//                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//                    D3D12_HEAP_FLAG_NONE,
//                    &CD3DX12_RESOURCE_DESC::Buffer(uploadSize),
//                    D3D12_RESOURCE_STATE_GENERIC_READ,
//                    nullptr,
//                    IID_PPV_ARGS(&m_uploadHeap));
//                if (FAILED(hr))
//                {
//#ifdef _DEBUG
//                    Core::DebugPrintF("ID3D12Texture::VInitFromFile, Failed to create texture upload heap.\n");
//#endif
//                    return false;
//                }
//
//                return true;
            

            /*void D3D12Texture::VOnLoaded()
            {

            }*/

            //void D3D12Texture::Upload(ID3D12GraphicsCommandList* commandList)
            //{
            //    //D3D12_SUBRESOURCE_DATA data = {};
            //    //data.pData = m_bitmap->GetData();
            //    //data.RowPitch = m_bitmap->GetWidth() * m_bitmap->GetBPP();
            //    //data.SlicePitch = data.RowPitch * m_bitmap->GetHeight();

            //    //UpdateSubresources(commandList, m_texture, m_uploadHeap, 0, 0, 1, &data);


            //    //commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            //    //// Describe and create a SRV for the texture.
            //    //D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            //    //srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            //    //srvDesc.Format = m_desc.Format;
            //    //srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            //    //srvDesc.Texture2D.MipLevels = 1;
            //    //m_device->CreateShaderResourceView(m_texture, &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());
            //}

            
        }
    }
}
