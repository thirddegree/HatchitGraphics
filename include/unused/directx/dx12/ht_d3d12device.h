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
#include <ht_device.h>
#include <ht_directx.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12Device : public IDevice
            {
            public:
                D3D12Device();

                ~D3D12Device();

                bool VInitialize()          override;

                void VReportDeviceInfo()    override;

                ID3D12Device*       GetDevice();
                ID3D12CommandQueue* GetQueue();
                uint32_t            GetRTVHeapIncrement();
                uint32_t            GetDSVHeapIncrement();
                uint32_t            GetSamHeapIncrement();
                uint32_t            GetCBVHeapIncrement();

            private:
                ID3D12Device*                   m_device;
                IDXGIAdapter3*                  m_adapter;
                IDXGIFactory4*                  m_factory;
                bool                            m_initialized;
                uint32_t                        m_RTVHeapIncrement;
                uint32_t                        m_DSVHeapIncrement;
                uint32_t                        m_SamHeapIncrement;
                uint32_t                        m_CBVHeapIncrement;
                DXGI_QUERY_VIDEO_MEMORY_INFO    m_vminfo;
                ID3D12CommandQueue*             m_queue;

                static bool         _DebugInterfaceEnabled;

                static HRESULT CheckHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter3** adapter);
            };
        }
    }
}