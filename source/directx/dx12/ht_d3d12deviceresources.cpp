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

#include <ht_d3d12deviceresources.h>
#include <ht_debug.h>
#include <wrl.h>

namespace Hatchit {
    namespace Graphics {

        namespace DX
        {
            D3D12DeviceResources::D3D12DeviceResources()
            {
                m_device = nullptr;
                m_swapChain = nullptr;
                m_renderTargetViewHeap = nullptr;
                m_commandAllocator = nullptr;
                m_commandQueue = nullptr;
               
                m_fence = nullptr;
              
                for (int i = 0; i < NUM_RENDER_TARGETS; i++)
                    m_renderTargets[i] = nullptr;

                m_currentFrame = 1;
            }

            D3D12DeviceResources::~D3D12DeviceResources()
            {
                ReleaseCOM(m_device);
                ReleaseCOM(m_swapChain);
                ReleaseCOM(m_renderTargetViewHeap);
                ReleaseCOM(m_depthStencilHeap);
                ReleaseCOM(m_commandQueue);
                ReleaseCOM(m_commandAllocator);
                ReleaseCOM(m_fence);
                for (int i = 0; i < NUM_RENDER_TARGETS; i++)
                    ReleaseCOM(m_renderTargets[i]);
                ReleaseCOM(m_depthStencil);
                CloseHandle(m_fenceEvent);
            }

            bool D3D12DeviceResources::Initialize(HWND hwnd, uint32_t width, uint32_t height)
            {
                return CreateDeviceResources(hwnd, width, height);
            }

            ID3D12Device * D3D12DeviceResources::GetDevice()
            {
                return m_device;
            }

            IDXGISwapChain3 * D3D12DeviceResources::GetSwapChain()
            {
                return m_swapChain;
            }

            ID3D12Resource * D3D12DeviceResources::GetRenderTarget()
            {
                return m_renderTargets[m_currentFrame];
            }

            ID3D12Resource * D3D12DeviceResources::GetDepthStencil()
            {
                return m_depthStencil;
            }

            ID3D12CommandAllocator * D3D12DeviceResources::GetCommandAllocator()
            {
                return m_commandAllocator;
            }

            ID3D12CommandQueue * D3D12DeviceResources::GetCommandQueue()
            {
                return m_commandQueue;
            }

            D3D12_VIEWPORT D3D12DeviceResources::GetScreenViewport()
            {
                return m_viewport;
            }

            CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DeviceResources::GetRenderTargetView()
            {
                return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), m_currentFrame, m_renderTargetViewHeapSize);
            }

            CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DeviceResources::GetDepthStencilView()
            {
                return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_depthStencilHeap->GetCPUDescriptorHandleForHeapStart());
            }

            uint32_t D3D12DeviceResources::GetCurrentFrameIndex()
            {
                return m_currentFrame;
            }

            bool D3D12DeviceResources::CreateDeviceResources(HWND hwnd, uint32_t width, uint32_t height)
            {
                HRESULT hr = S_OK;

                /*
                * Initialize the D3D12 Debug Layer
                */
#ifdef _DEBUG
                Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
                if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
                {
                    debugController->EnableDebugLayer();
                }
#endif
                IDXGIFactory4* factory = nullptr;
                hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to create DXGIFactory.\n");
#endif
                    ReleaseCOM(factory);
                    return false;
                }

                IDXGIAdapter1* hardwareAdapter = nullptr;
                hr = CheckHardwareAdapter(factory, &hardwareAdapter);
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to find suitable Direct3D12 adapter.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }

                /*
                * Create the device
                */
                hr = D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to create Direct3D12 device.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }

                /**
                * Describe and create the command queue
                *
                * NOTE:
                *  Specifying COMMAND_LIST_TYPE_DIRECT means
                *  this command buffer is only available for use with direct
                *  GPU calls.
                */
                D3D12_COMMAND_QUEUE_DESC queueDesc = {};
                queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

                hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to create Direct3D12 Command Queue.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }



                
                DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
                swapChainDesc.BufferCount = NUM_RENDER_TARGETS; //double buffered
                swapChainDesc.BufferDesc.Width = width;
                swapChainDesc.BufferDesc.Height = height;
                swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                swapChainDesc.OutputWindow = hwnd;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.Windowed = true;


                /**
                * NOTE:
                * New to Direct3D12, the swap chain must now be passed
                * the command queue rather than the device. As it will operate and flush
                * the command queue directly.
                */
                IDXGISwapChain* swapChain;
                hr = factory->CreateSwapChain(m_commandQueue, &swapChainDesc, &swapChain);
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to create swapChain.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }

                /*
                * Attempt to query a IDXGISwapChain3 interface from swap chain
                */
                hr = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), reinterpret_cast<void**>(&m_swapChain));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to query IDXGISwapChain3 interface.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }
                ReleaseCOM(swapChain);
                m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();

                /*
                * Create desciptor heaps
                */
                D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
                rtvHeapDesc.NumDescriptors = 2;
                rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_renderTargetViewHeap));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to create render target view heap.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }
                m_renderTargetViewHeapSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

                /*
                * Now we will create the resources for each frame
                */
                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
                for (uint32_t i = 0; i < NUM_RENDER_TARGETS; i++)
                {
                    hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
                    if (FAILED(hr))
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to get render target buffer for resource creation.\n");
#endif
                        ReleaseCOM(hardwareAdapter);
                        ReleaseCOM(factory);
                        return false;
                    }

                    m_device->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);

                    rtvHandle.ptr += 1 * m_renderTargetViewHeapSize;
                }

                /*
                * Create a depth stencil view
                */
                D3D12_DESCRIPTOR_HEAP_DESC depthStencilHeapDesc = {};
                depthStencilHeapDesc.NumDescriptors = 1;
                depthStencilHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
                depthStencilHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                hr = m_device->CreateDescriptorHeap(&depthStencilHeapDesc, IID_PPV_ARGS(&m_depthStencilHeap));

                D3D12_HEAP_PROPERTIES depthHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
                D3D12_RESOURCE_DESC   depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                    width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
                D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
                depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
                depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
                depthOptimizedClearValue.DepthStencil.Stencil = 0;

                hr = m_device->CreateCommittedResource(
                    &depthHeapProps,
                    D3D12_HEAP_FLAG_NONE,
                    &depthResourceDesc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    &depthOptimizedClearValue,
                    IID_PPV_ARGS(&m_depthStencil));

                D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
                depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
                depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

                m_device->CreateDepthStencilView(m_depthStencil, &depthStencilViewDesc, m_depthStencilHeap->GetCPUDescriptorHandleForHeapStart());

                /*
                * Create a command allocator for managing memory for command list
                */
                hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    Core::DebugPrintF("D3D12Renderer::VInitialize(), Failed to create command allocator for command list memory allocation.\n");
#endif
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }

                ReleaseCOM(hardwareAdapter);
                ReleaseCOM(factory);

                // Create synchronization objects.
                m_device->CreateFence(m_fenceValues[m_currentFrame], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
                m_fenceValues[m_currentFrame]++;

                m_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

                m_viewport.Width = static_cast<float>(width);
                m_viewport.Height = static_cast<float>(height);
                m_viewport.TopLeftX = 0.0f;
                m_viewport.TopLeftY = 0.0f;
                m_viewport.MinDepth = 0.0f;
                m_viewport.MaxDepth = 1.0f;

                m_scissorRect.left = 0;
                m_scissorRect.top = 0;
                m_scissorRect.right = static_cast<LONG>(width);
                m_scissorRect.bottom = static_cast<LONG>(height);

                return true;
            }

            void D3D12DeviceResources::Present()
            {
                // The first argument instructs DXGI to block until VSync, putting the application
                // to sleep until the next VSync. This ensures we don't waste any cycles rendering
                // frames that will never be displayed to the screen.
                HRESULT hr = m_swapChain->Present(1, 0);

                // If the device was removed either by a disconnection or a driver upgrade, we 
                // must recreate all device resources.
                if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
                {
                    m_deviceRemoved = true;
                }
                else
                {
                    MoveToNextFrame();
                }
            }


            HRESULT D3D12DeviceResources::CheckHardwareAdapter(IDXGIFactory2 * pFactory, IDXGIAdapter1 ** ppAdapter)
            {
                HRESULT hr = S_OK;

                IDXGIAdapter1* _adapter;

                *ppAdapter = nullptr;

                /*
                * Iterate over all available adapters, and select the first
                * adapter that supports Direct3D 12
                */
                for (uint32_t index = 0; index < DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(index, &_adapter); index++)
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

                *ppAdapter = _adapter;

                return hr;
            }

            void D3D12DeviceResources::ValidateDevice()
            {
                // The D3D Device is no longer valid if the default adapter changed since the device
                // was created or if the device has been removed.

                // First, get the LUID for the adapter from when the device was created.

                LUID previousAdapterLuid = m_device->GetAdapterLuid();

                // Next, get the information for the current default adapter.

                IDXGIFactory2* currentFactory;
                CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory));

                IDXGIAdapter1* currentDefaultAdapter;
                currentFactory->EnumAdapters1(0, &currentDefaultAdapter);

                DXGI_ADAPTER_DESC currentDesc;
                currentDefaultAdapter->GetDesc(&currentDesc);

                // If the adapter LUIDs don't match, or if the device reports that it has been removed,
                // a new D3D device must be created.

                if (previousAdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
                    previousAdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
                    FAILED(m_device->GetDeviceRemovedReason()))
                {
                    m_deviceRemoved = true;
                }

                ReleaseCOM(currentFactory);
            }

            void D3D12DeviceResources::WaitForGPU()
            {
                // Schedule a Signal command in the queue.
                m_commandQueue->Signal(m_fence, m_fenceValues[m_currentFrame]);

                // Wait until the fence has been crossed.
                m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrame], m_fenceEvent);
                WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

                // Increment the fence value for the current frame.
                m_fenceValues[m_currentFrame]++;
            }

            void D3D12DeviceResources::MoveToNextFrame()
            {
                // Schedule a Signal command in the queue.
                const UINT64 currentFenceValue = m_fenceValues[m_currentFrame];
                m_commandQueue->Signal(m_fence, currentFenceValue);

                // Advance the frame index.
                m_currentFrame = (m_currentFrame + 1) % NUM_BUFFER_FRAMES;

                // Check to see if the next frame is ready to start.
                if (m_fence->GetCompletedValue() < m_fenceValues[m_currentFrame])
                {
                    m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrame], m_fenceEvent);
                    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
                }

                // Set the fence value for the next frame.
                m_fenceValues[m_currentFrame] = currentFenceValue + 1;
            }
        }
    }
}