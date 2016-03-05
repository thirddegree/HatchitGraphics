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

                bool Initialize(ID3D12Device* device);

            private:
                BYTE*                               m_data;

                D3D12_CONSTANT_BUFFER_VIEW_DESC     m_view;
                ID3D12Resource*                     m_buffer;
                ID3D12DescriptorHeap*               m_heap;
            };
        }
    }
}