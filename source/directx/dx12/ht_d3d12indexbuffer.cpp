/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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

#include <ht_d3d12indexbuffer.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX {

            D3D12IndexBuffer::D3D12IndexBuffer(uint32_t size)
            {
                m_bufferSize = size * sizeof(unsigned short);
                m_buffer = nullptr;
                m_bufferUploadHeap = nullptr;
            }

            D3D12IndexBuffer::~D3D12IndexBuffer()
            {
                ReleaseCOM(m_buffer);
                ReleaseCOM(m_bufferUploadHeap);
            }

            bool D3D12IndexBuffer::Initialize(ID3D12Device* device)
            {
                HRESULT hr = S_OK;

                CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
                CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);
                hr = device->CreateCommittedResource(
                    &defaultHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_COPY_DEST,
                    nullptr,
                    IID_PPV_ARGS(&m_buffer));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("D3D12IndexBuffer::Initialize(), Failed to create buffer.\n");
                    return false;
                }

                CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
                hr = device->CreateCommittedResource(
                    &uploadHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_bufferUploadHeap));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("D3D12IndexBuffer::Initialize(), Failed to create buffer upload heap.\n");
                    return false;
                }

                return true;
            }

            bool D3D12IndexBuffer::UpdateSubData(ID3D12GraphicsCommandList* commandList, uint32_t offset, uint32_t count, const void* data)
            {
                D3D12_SUBRESOURCE_DATA _data;
                _data.pData = reinterpret_cast<const BYTE*>(data);
                _data.RowPitch = m_bufferSize;
                _data.SlicePitch = _data.RowPitch;

                UpdateSubresources(commandList, m_buffer, m_bufferUploadHeap, 0, 0, 1, &_data);

                CD3DX12_RESOURCE_BARRIER bufferResourceBarrier =
                    CD3DX12_RESOURCE_BARRIER::Transition(m_buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
                commandList->ResourceBarrier(1, &bufferResourceBarrier);

                m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
                m_view.SizeInBytes = m_bufferSize;
                m_view.Format = DXGI_FORMAT_R16_UINT;
               

                return true;
            }

            D3D12_INDEX_BUFFER_VIEW D3D12IndexBuffer::GetView()
            {
                return m_view;
            }
        }
    }
}
