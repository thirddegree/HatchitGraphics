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

#include <ht_d3d12constantbuffer.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            D3D12ConstantBuffer::D3D12ConstantBuffer()
            {
                m_heap = nullptr;
                m_data = nullptr;
                m_buffer = nullptr;
            }

            D3D12ConstantBuffer::~D3D12ConstantBuffer()
            {
                ReleaseCOM(m_heap);
                ReleaseCOM(m_buffer);
            }

            bool D3D12ConstantBuffer::Initialize(ID3D12Device* device)
            {
                if (!device)
                    return false;

                HRESULT hr = S_OK;

                /*
                * Describe and create a constant buffer view (CBV) descriptor heap
                *  Flags indicate that this descriptor heap can be bound to the pipeline 
		        *  and that descriptors contained in it can be referenced by a root table.
                */
                D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
                heapDesc.NumDescriptors = 1;
                heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
                if (FAILED(hr))
                {
#ifdef _DEBUG

#endif
                    return false;
                }

                return true;
            }


        }
    }
}