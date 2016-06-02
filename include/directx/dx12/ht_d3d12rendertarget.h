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
#include <ht_rendertarget_base.h>
#include <ht_rendertarget_resource.h>
#include <ht_refcounted.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class D3D12DeviceResources;

            class HT_API D3D12RenderTarget : public RenderTargetBase
            {
            public:
                D3D12RenderTarget();

                bool Initialize(const Resource::RenderTargetHandle& handle);

            private:
                ID3D12Resource*                 m_resource;
                D3D12_CPU_DESCRIPTOR_HANDLE     m_cpuHandle;
                D3D12_RESOURCE_DESC             m_desc;

                static DXGI_FORMAT TargetFormatFromString(std::string s);
            };
        }
    }
}