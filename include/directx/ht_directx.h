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

            struct Vertex
            {
                Math::Float3 position;
                Math::Float3 normal;
                Math::Float4 color;
            };
            
        }

    }

}