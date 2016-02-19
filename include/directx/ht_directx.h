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

#include <d3d11.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

namespace Hatchit {

    namespace Graphics {

        namespace DirectX
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

            inline void ThrowIfFailed(HRESULT hr)
            {
                if (FAILED(hr))
                    throw;
            }
        }

    }

}