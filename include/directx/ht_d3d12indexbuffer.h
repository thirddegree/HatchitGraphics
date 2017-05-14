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

#pragma once

#include <ht_platform.h>
#include <ht_directx.h>
#include <cstdint>

namespace Hatchit {

    namespace Graphics {

        namespace DX {

            class HT_API D3D12IndexBuffer
            {
            public:
                D3D12IndexBuffer(uint32_t size);

                ~D3D12IndexBuffer();

                bool Initialize(ID3D12Device* device);

                bool UpdateSubData(ID3D12GraphicsCommandList* commandList, uint32_t offset, uint32_t count, const void* data);

                D3D12_INDEX_BUFFER_VIEW GetView();

            private:
                ID3D12Resource*         m_buffer;
                ID3D12Resource*         m_bufferUploadHeap;
                D3D12_INDEX_BUFFER_VIEW m_view;
                uint32_t                m_bufferSize;
            };
        }
    }
}