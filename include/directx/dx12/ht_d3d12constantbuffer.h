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


namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            class HT_API D3D12ConstantBuffer
            {
            public:
                D3D12ConstantBuffer();

                ~D3D12ConstantBuffer();

                bool Initialize(ID3D12Device* device,
                    ID3D12DescriptorHeap* heap,
                    uint64_t size);

                void Map(uint32_t subresource, uint64_t size);
                void Fill(void** data, size_t size, size_t cbSize, uint32_t currentFrame);
                void Unmap(uint32_t subresource);

                const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUAddress();

            public:
                UINT8*                              m_mappedData;
                D3D12_CONSTANT_BUFFER_VIEW_DESC     m_view;
                D3D12_GPU_VIRTUAL_ADDRESS           m_gpuAddress;
                ID3D12Resource*                     m_buffer;
            };
        }
    }
}