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
#include <ht_rootlayout_resource.h>
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
                m_commandAllocators[0] = nullptr;
                m_commandAllocators[1] = nullptr;
                m_commandQueue = nullptr;
                m_commandList = nullptr;
               
                m_fence = nullptr;
              
                for (int i = 0; i < NUM_BUFFER_FRAMES; i++)
                    m_renderTargets[i] = nullptr;

                m_currentFrame = 0;
                m_currentFence = 0;
            }

            D3D12DeviceResources::~D3D12DeviceResources()
            {
                ReleaseCOM(m_device);
                ReleaseCOM(m_swapChain);
                ReleaseCOM(m_renderTargetViewHeap);
                ReleaseCOM(m_depthStencilHeap);
                ReleaseCOM(m_commandQueue);
                ReleaseCOM(m_commandList);
                ReleaseCOM(m_fence);
                for (int i = 0; i < NUM_BUFFER_FRAMES; i++)
                {
                    ReleaseCOM(m_renderTargets[i]);
                    ReleaseCOM(m_commandAllocators[i]);
                }
                ReleaseCOM(m_depthStencil);
            }

            bool D3D12DeviceResources::Initialize(HWND hwnd, uint32_t width, uint32_t height)
            {
				m_hwnd = hwnd;

                return CreateDeviceResources(hwnd, width, height);
            }

            ID3D12Device * D3D12DeviceResources::GetDevice()
            {
                return m_device;
            }

            D3D12RootLayoutHandle D3D12DeviceResources::GetRootLayout()
            {
                return m_rootLayout;
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
                return m_commandAllocators[m_currentFrame];
            }

            ID3D12CommandQueue * D3D12DeviceResources::GetCommandQueue()
            {
                return m_commandQueue;
            }

            ID3D12GraphicsCommandList * D3D12DeviceResources::GetCommandList()
            {
                return m_commandList;
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

			void D3D12DeviceResources::DestroyDeviceResources()
			{
				ReleaseCOM(m_renderTargetViewHeap);
				ReleaseCOM(m_depthStencilHeap);
				for (int i = 0; i < NUM_BUFFER_FRAMES; i++)
				{
					ReleaseCOM(m_renderTargets[i]);
				}
                m_currentFence = 0;
				ReleaseCOM(m_depthStencil);
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
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create DXGIFactory.\n");
                    ReleaseCOM(factory);
                    return false;
                }

                IDXGIAdapter1* hardwareAdapter = nullptr;
                hr = CheckHardwareAdapter(factory, &hardwareAdapter);
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to find suitable Direct3D12 adapter.\n");
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
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create Direct3D12 device.\n");
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
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create Direct3D12 Command Queue.\n");
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }



                
                m_swapChainDesc = {};
				m_swapChainDesc.BufferCount = NUM_BUFFER_FRAMES; //double buffered
				m_swapChainDesc.BufferDesc.Width = width;
				m_swapChainDesc.BufferDesc.Height = height;
				m_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				m_swapChainDesc.OutputWindow = hwnd;
				m_swapChainDesc.SampleDesc.Count = 1;
				m_swapChainDesc.Windowed = true;


                /**
                * NOTE:
                * New to Direct3D12, the swap chain must now be passed
                * the command queue rather than the device. As it will operate and flush
                * the command queue directly.
                */
                IDXGISwapChain* swapChain;
                hr = factory->CreateSwapChain(m_commandQueue, &m_swapChainDesc, &swapChain);
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create swapChain.\n");
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
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to query IDXGISwapChain3 interface.\n");
                    ReleaseCOM(hardwareAdapter);
                    ReleaseCOM(factory);
                    return false;
                }
                ReleaseCOM(swapChain);
                m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();

				if (!CreateBuffers(width, height))
					return false;

                /*
                * Create a command allocator for managing memory for command list
                */
                for (int i = 0; i < NUM_BUFFER_FRAMES; i++)
                {
                    hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));
                    if (FAILED(hr))
                    {
                        HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create command allocator for command list memory allocation.\n");
                        ReleaseCOM(hardwareAdapter);
                        ReleaseCOM(factory);
                        return false;
                    }
                }
                

                ReleaseCOM(hardwareAdapter);
                ReleaseCOM(factory);

                /*Create a command list*/
                hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_currentFrame],
                    nullptr, IID_PPV_ARGS(&m_commandList));
                if (FAILED(hr))
                    return false;
                m_commandList->Close();

                // Create synchronization objects.
                m_device->CreateFence(m_fenceValues[m_currentFrame], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
                m_fenceValues[m_currentFrame]++;
                m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                

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

				Resource::RootLayoutHandle handle = Resource::RootLayout::GetHandleFromFileName("TestRootDescriptor.json");
                m_rootLayout = D3D12RootLayout::GetHandle("TestRootDescriptor.json", "TestRootDescriptor.json", m_device);

                return true;
            }

			bool D3D12DeviceResources::CreateBuffers(uint32_t width, uint32_t height)
			{
				HRESULT hr = S_OK;

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
					HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create render target view heap.\n");
					return false;
				}
				m_renderTargetViewHeapSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

				/*
				* Now we will create the resources for each frame
				*/
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
				for (uint32_t i = 0; i < NUM_BUFFER_FRAMES; i++)
				{
					hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
					if (FAILED(hr))
					{
						HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to get render target buffer for resource creation.\n");
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

        

				return true;
			}
			
			void D3D12DeviceResources::Resize(uint32_t width, uint32_t height)
			{
				m_viewport.Width = static_cast<float>(width);
				m_viewport.Height = static_cast<float>(height);

                WaitForGpu();

                m_commandList->Reset(m_commandAllocators[m_currentFrame], nullptr);

				DestroyDeviceResources();

				m_swapChain->ResizeBuffers(2, width, height, m_swapChainDesc.BufferDesc.Format, 0);
				m_swapChain->GetDesc(&m_swapChainDesc);

				this->CreateBuffers(width, height);

                //Close and execute the command list
                ExecuteCommandList();
                
                MoveToNextFrame();
			}

            void D3D12DeviceResources::Present()
            {
                // The first argument instructs DXGI to block until VSync, putting the application
                // to sleep until the next VSync.
                HRESULT hr = m_swapChain->Present(0, 0);
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

            //void D3D12DeviceResources::FlushCommandQueue()
            //{
            //    //Advance fence value to mark commands made up to this point
            //    m_currentFence++;

            //    HRESULT hr = S_OK;

            //    /*
            //    * Add an instruction to the command queue to set a fence point. Since we are on the GPU
            //    * timeline, the new fence point wont be set until the GPU finished processing all commands
            //    * prior to this call to Signal()
            //    */
            //    hr = m_commandQueue->Signal(m_fence, m_currentFence);

            //    //Wait until GPU has completed events to this fence point
            //    if (m_fence->GetCompletedValue() < m_currentFence)
            //    {
            //        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

            //        //Fire event when GPU hits current fence
            //        hr = m_fence->SetEventOnCompletion(m_currentFence, eventHandle);

            //        WaitForSingleObject(eventHandle, INFINITE);
            //        CloseHandle(eventHandle);
            //    }
            //}

            void D3D12DeviceResources::WaitForGpu()
            {
                m_commandQueue->Signal(m_fence, m_fenceValues[m_currentFrame]);

                //Wait until the fence has been processed
                m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrame], m_fenceEvent);
                WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

                m_fenceValues[m_currentFrame]++;
            }

            void D3D12DeviceResources::ExecuteCommandList()
            {
                m_commandList->Close();
                ID3D12CommandList* cmdsLists[] = { m_commandList };
                m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
            }

            void D3D12DeviceResources::MoveToNextFrame()
            {
                // Schedule a Signal command in the queue.
                const UINT64 currentFenceValue = m_fenceValues[m_currentFrame];
                ThrowIfFailed(m_commandQueue->Signal(m_fence, currentFenceValue));

                // Update the frame index.
                m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();

                // If the next frame is not ready to be rendered yet, wait until it is ready.
                if (m_fence->GetCompletedValue() < m_fenceValues[m_currentFrame])
                {
                    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrame], m_fenceEvent));
                    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
                }

                // Set the fence value for the next frame.
                m_fenceValues[m_currentFrame] = currentFenceValue + 1;
            }
        }
    }
}
