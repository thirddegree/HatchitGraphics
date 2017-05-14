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

#include <ht_d3d12constantbuffer.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            D3D12ConstantBuffer::D3D12ConstantBuffer()
            {
                m_gpuAddress = 0;
                m_buffer = nullptr;
               
            }

            D3D12ConstantBuffer::~D3D12ConstantBuffer()
            {
                ReleaseCOM(m_buffer);
            }

            bool D3D12ConstantBuffer::Initialize(ID3D12Device* device, ID3D12DescriptorHeap* heap, uint64_t size)
            {
                if (!device)
                    return false;

                HRESULT hr = S_OK;

                CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DX::ConstantBufferByteSize(static_cast<UINT>(size)));
                hr = device->CreateCommittedResource(
                    &uploadHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &constantBufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_buffer));

                m_gpuAddress = m_buffer->GetGPUVirtualAddress();

                CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(heap->GetCPUDescriptorHandleForHeapStart());
                m_view.BufferLocation = m_gpuAddress;
                m_view.SizeInBytes = DX::ConstantBufferByteSize(static_cast<UINT>(size));
                device->CreateConstantBufferView(&m_view, cpuHandle);

                return true;
            }

            void D3D12ConstantBuffer::Map(uint32_t subresource, uint64_t size)
            {
                HRESULT hr = S_OK;

                hr = m_buffer->Map(subresource, nullptr, reinterpret_cast<void**>(&m_mappedData));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to map constant buffer.\n");
                    return;
                }
                ZeroMemory(m_mappedData, size);
            }

            void D3D12ConstantBuffer::Fill(void** data, size_t size, size_t cbSize, uint32_t currentFrame)
            {
                UINT8* destination = m_mappedData + (currentFrame * DX::ConstantBufferByteSize(static_cast<UINT>(cbSize)));

                memcpy(destination, &data[0], size);
            }

            void D3D12ConstantBuffer::Unmap(uint32_t subresource)
            {
                m_buffer->Unmap(subresource, nullptr);
            }

            const D3D12_GPU_VIRTUAL_ADDRESS & D3D12ConstantBuffer::GetGPUAddress()
            {
                return m_gpuAddress;
            }


        }
    }
}