/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 ThirdDegree
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
#include <ht_rendertarget.h>
#include <ht_rendertarget_resource.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12RenderTarget : public IRenderTarget
            {
            public:
                D3D12RenderTarget();

                bool Initialize(Resource::RenderTargetHandle handle, ID3D12Device* device);

            private:
                ID3D12Resource* m_resource;
            };
        }
    }
}