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

#include <ht_d3d12device.h>
#include <ht_debug.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            bool        D3D12Device::_DebugInterfaceEnabled = false;

            D3D12Device::D3D12Device()
            {
                m_device = nullptr;
                m_adapter = nullptr;
                m_factory = nullptr;
                m_queue = nullptr;
                m_initialized = false;
            }

            D3D12Device::~D3D12Device()
            {
                ReleaseCOM(m_adapter);
                ReleaseCOM(m_factory);
                ReleaseCOM(m_device);
                ReleaseCOM(m_queue);
            }

            ID3D12Device* D3D12Device::GetDevice()
            {
                return m_device;
            }

            ID3D12CommandQueue * D3D12Device::GetQueue()
            {
                return m_queue;
            }

            uint32_t D3D12Device::GetRTVHeapIncrement()
            {
                return m_RTVHeapIncrement;
            }

            uint32_t D3D12Device::GetDSVHeapIncrement()
            {
                return m_DSVHeapIncrement;
            }

            uint32_t D3D12Device::GetSamHeapIncrement()
            {
                return m_SamHeapIncrement;
            }

            uint32_t D3D12Device::GetCBVHeapIncrement()
            {
                return m_CBVHeapIncrement;
            }

            bool D3D12Device::VInitialize()
            {
                HRESULT hr = S_OK;

                /*
                * Enable debug interface if applicable.
                */
#ifdef _DEBUG
                if (!_DebugInterfaceEnabled)
                {
                    ID3D12Debug* debugInterface = nullptr;
                    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
                    {
                        debugInterface->EnableDebugLayer();
                        _DebugInterfaceEnabled = true;
                    }
                    ReleaseCOM(debugInterface);
                }
#endif
                hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
                if (FAILED(hr))
                {   
                    HT_ERROR_PRINTF("Failed to create DXGIFactory for device creation.\n");
                    return false;
                }

                /*Find GPU adapter*/
                hr = CheckHardwareAdapter(m_factory, &m_adapter);
                if (FAILED(hr))
                {
                    HT_ERROR_PRINTF("Failed to find suitable Direct3D 12 adapter.\n");
                    return false;
                }

                /*Create the device*/
                hr = D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
                if (FAILED(hr))
                {
                    HT_ERROR_PRINTF("Failed to create Direct3D 12 device.\n");
                    return false;
                }

                /*Cache heap increments*/
                m_RTVHeapIncrement = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                m_DSVHeapIncrement = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                m_CBVHeapIncrement = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                m_SamHeapIncrement = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

                /*Create the command queue*/
                D3D12_COMMAND_QUEUE_DESC queueDesc = {};
                queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queue));
                if (FAILED(hr))
                {
                    HT_ERROR_PRINTF("Failed to create Direct3D 12 Command Queue.\n");
                    return false;
                }

                m_initialized = true;

                return true;
            }

            void D3D12Device::VReportDeviceInfo()
            {
                if (!m_initialized)
                    return;

                
            }

            HRESULT D3D12Device::CheckHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter3** adapter)
            {
                HRESULT hr = S_OK;

                IDXGIAdapter1* _adapter;

                *adapter = nullptr;

                /*
                * Iterate over all available adapters, and select the first
                * adapter that supports Direct3D 12
                */
                for (uint32_t index = 0; index < DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(index, &_adapter); index++)
                {
                    DXGI_ADAPTER_DESC1 desc;
                    _adapter->GetDesc1(&desc);

                    /**
                    * We do not want to select the "Microsoft Basic Render Driver" adapter
                    * as this is a default render-only adapter with no outputs. WARP should
                    * be used instead for software device
                    */
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                        continue;

                    /**
                    * Check to see if the adapter (GPU) supports Direct3D 12
                    */
                    hr = D3D12CreateDevice(_adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
                    if (SUCCEEDED(hr))
                        break;
                }
                hr = _adapter->QueryInterface(IID_PPV_ARGS(&*adapter));

                ReleaseCOM(_adapter);

                return hr;
            }
        }
    }
}