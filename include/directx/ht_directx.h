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

#ifdef DX11_SUPPORT
#include <d3d11.h>
#endif

#ifdef DX12_SUPPORT
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dx12.h>
#endif

#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <ht_math.h>

#ifndef HT_D3D12_DEBUGNAME
    #if defined(_DEBUG) || defined(D3D12_ASSIGN_DEBUG_NAMES)
    #define HT_D3D12_DEBUGNAME(object, name) Hatchit::Graphics::DX::RegisterDebugName(object, name)
    #else
    #define HT_D3D12_DEBUGNAME(object, name)
    #endif
#endif

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            template <typename T>
            void ReleaseCOM(T* t)
            {
                if (t)
                {
                    t->Release();
                    t = nullptr;
                }
            }

            inline
            void RegisterDebugName(ID3D12Object* object, LPCSTR name)
            {
                std::wstringstream ss;
                ss << name;
                if(object)
                {
                    object->SetName(ss.str().c_str());
                }
            }

            inline void ThrowIfFailed(HRESULT hr)
            {
                if (FAILED(hr))
                    throw;
            }

            inline UINT ConstantBufferByteSize(UINT byteSize)
            {
                // Constant buffers must be a multiple of the minimum hardware
                // allocation size (usually 256 bytes).  So round up to nearest
                // multiple of 256.  We do this by adding 255 and then masking off
                // the lower 2 bytes which store all bits < 256.
                // Example: Suppose byteSize = 300.
                // (300 + 255) & ~255
                // 555 & ~255
                // 0x022B & ~0x00ff
                // 0x022B & 0xff00
                // 0x0200
                // 512
                return (byteSize + 255) & ~255;
            }

            struct Vertex
            {
                Math::Float3 position;
                Math::Float3 normal;
                Math::Float4 color;
            };
            
        }

    }

}