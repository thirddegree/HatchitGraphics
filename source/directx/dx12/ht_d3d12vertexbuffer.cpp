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

#include <ht_d3d12vertexbuffer.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace DirectX {
            
            D3D12VertexBuffer::D3D12VertexBuffer(uint32_t size)
            {
                m_bufferSize = size;
            }

            D3D12VertexBuffer::~D3D12VertexBuffer()
            {
                DirectX::ReleaseCOM(m_buffer);
            }

            bool D3D12VertexBuffer::Initialize(ID3D12Device* device)
            {
                HRESULT hr = S_OK;

                hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_buffer));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12VertexBuffer::Initialize(), Failed to create buffer.\n");
#endif
                    return false;
                }

                return true;
            }

            bool D3D12VertexBuffer::UpdateSubData(uint32_t offset, uint32_t count, const void* data)
            {
                HRESULT hr = S_OK;

                CD3DX12_RANGE range(0, 0);
                uint8_t*      temp;
                hr = m_buffer->Map(0, &range, reinterpret_cast<void**>(&temp));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12VertexBuffer::UpdateSubData(), Failed to map buffer.\n");
#endif
                    return false;
                }

                /*copy data into buffer*/
                memcpy(temp, data, sizeof(Vertex) * count);

                m_buffer->Unmap(0, nullptr);

                /*initialize buffer view*/
                m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
                m_view.StrideInBytes = sizeof(Vertex);
                m_view.SizeInBytes = count * sizeof(Vertex);
            }

            D3D12_VERTEX_BUFFER_VIEW D3D12VertexBuffer::GetView()
            {
                return m_view;
            }

        }
    }
}