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

#include <ht_d3d12swapchain.h>
#include <ht_renderer.h>
#include <ht_d3d12device.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12SwapChain::D3D12SwapChain(HWND hwnd)
            {
                m_hwnd = hwnd;
                m_chain = nullptr;
                for (uint32_t i = 0; i < NUM_BUFFER_FRAMES; i++)
                {
                    m_commandAllocators[i] = nullptr;
                    m_renderTargets[i] = nullptr;
                }
                m_depthStencilTarget = nullptr;
                m_depthStencilTargetHeap = nullptr;
                m_renderTargetHeap = nullptr;
                m_commandList = nullptr;
            }

            D3D12SwapChain::~D3D12SwapChain()
            {
                ReleaseCOM(m_depthStencilTarget);
                ReleaseCOM(m_depthStencilTargetHeap);
                ReleaseCOM(m_renderTargetHeap);
                for (uint32_t i = 0; i < NUM_BUFFER_FRAMES; i++)
                {
                    ReleaseCOM(m_commandAllocators[i]);
                    ReleaseCOM(m_renderTargets[i]);
                }
                ReleaseCOM(m_chain);
            }

            bool D3D12SwapChain::VInitialize(uint32_t width, uint32_t height)
            {
                HRESULT hr = S_OK;

                auto device = static_cast<D3D12Device*>(Renderer::GetDevice())->GetDevice();

                IDXGIFactory4* factory = nullptr;
                hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
                if (FAILED(hr))
                {
                    HT_ERROR_PRINTF("Failed to create DXGIFactory.\n");
                    return false;
                }

                DXGI_SWAP_CHAIN_DESC desc = {};
                desc.BufferCount = NUM_BUFFER_FRAMES;
                desc.BufferDesc.Width = width;
                desc.BufferDesc.Height = height;
                desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                desc.SampleDesc.Count = 1;
                desc.OutputWindow = m_hwnd;
                desc.Windowed = true;

                IDXGISwapChain* chain;
                hr = factory->CreateSwapChain(static_cast<D3D12Device*>(Renderer::GetDevice())->GetQueue(),
                    &desc, &chain);
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create DXGISwapChain.\n");
                    ReleaseCOM(factory);
                    return false;
                }
                ReleaseCOM(factory);
                
                hr = chain->QueryInterface(IID_PPV_ARGS(&m_chain));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to initialize DXGISwapChain3 interface.\n");
                    return false;
                }
                ReleaseCOM(chain);
                m_currentBuffer = m_chain->GetCurrentBackBufferIndex();

                /*
                * Create a command allocator for managing memory for command list
                */
                for (int i = 0; i < NUM_BUFFER_FRAMES; i++)
                {
                    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));
                    if (FAILED(hr))
                    {
                        HT_DEBUG_PRINTF("Failed to create command allocator for command list memory allocation.\n");
                        return false;
                    }
                }

                hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0], nullptr, IID_PPV_ARGS(&m_commandList));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create D3D12SwapChain command list.\n");
                    return false;
                }
                m_commandList->Close();
                
                //Create synchronization objects.
                hr = device->CreateFence(m_fenceValues[m_currentBuffer], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to created D3D12Fence object for swapchain.\n");
                    return false;
                }
                m_fenceValues[m_currentBuffer]++;
                m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

                if (!CreateBuffers(width, height))
                    return false;

                return true;
            }

            void D3D12SwapChain::VClear(float* color)
            {
                m_commandList->Close();
                m_commandList->Reset(m_commandAllocators[m_currentBuffer], nullptr);
                m_commandList->ClearRenderTargetView(GetRenderTargetView(), color, 0, nullptr);
                m_commandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            }

            void D3D12SwapChain::VResize(uint32_t width, uint32_t height)
            {
            }

            void D3D12SwapChain::VPresent()
            {
                HRESULT hr = S_OK;

                auto queue = static_cast<D3D12Device*>(Renderer::GetDevice())->GetQueue();

                ID3D12CommandList* commands = { m_commandList };
                queue->ExecuteCommandLists(1, &commands);
                hr = m_chain->Present(0, 0);
            }

            bool D3D12SwapChain::CreateBuffers(uint32_t width, uint32_t height)
            {
                HRESULT hr = S_OK;

                auto device = static_cast<D3D12Device*>(Renderer::GetDevice())->GetDevice();
        
                D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
                rtvHeapDesc.NumDescriptors = 2;
                rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_renderTargetHeap));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create D3D12SwapChain render target view heap.\n");
                    return false;
                }
                uint32_t heapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_renderTargetHeap->GetCPUDescriptorHandleForHeapStart());
                for (uint32_t i = 0; i < NUM_BUFFER_FRAMES; i++)
                {
                    hr = m_chain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
                    if (FAILED(hr))
                    {
                        HT_DEBUG_PRINTF("D3D12SwapChain, Failed to get render target buffer for resource creation.\n");
                        return false;
                    }
                    device->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);

                    rtvHandle.ptr += 1 * heapSize;
                }

                D3D12_DESCRIPTOR_HEAP_DESC dsHeapDesc = {};
                dsHeapDesc.NumDescriptors = 1;
                dsHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
                dsHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                hr = device->CreateDescriptorHeap(&dsHeapDesc, IID_PPV_ARGS(&m_depthStencilTargetHeap));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Faliled to create D3D12SwapChain depth stencil heap.\n");
                    return false;
                }


                D3D12_RESOURCE_DESC   depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                    width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
                D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
                depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
                depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
                depthOptimizedClearValue.DepthStencil.Stencil = 0;

                hr = device->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &depthResourceDesc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    &depthOptimizedClearValue,
                    IID_PPV_ARGS(&m_depthStencilTarget));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create D3D12SwapChain depth stencil target.\n");
                    return false;
                }

                D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
                depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
                depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
                device->CreateDepthStencilView(m_depthStencilTarget, &depthStencilViewDesc, m_depthStencilTargetHeap->GetCPUDescriptorHandleForHeapStart());

                return true;
            }

            void D3D12SwapChain::MoveToNextFrame()
            {
                HRESULT hr = S_OK;
                
                auto queue = static_cast<D3D12Device*>(Renderer::GetDevice())->GetQueue();

                const uint64_t currentFenceValue = m_fenceValues[m_currentBuffer];
                hr = queue->Signal(m_fence, currentFenceValue);
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to signal ID3D12CommandQueue");
                }

                m_currentBuffer = m_chain->GetCurrentBackBufferIndex();

                if (m_fence->GetCompletedValue() < m_fenceValues[m_currentBuffer])
                {
                    hr = m_fence->SetEventOnCompletion(m_fenceValues[m_currentBuffer], m_fenceEvent);
                    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
                }

                m_fenceValues[m_currentBuffer] = currentFenceValue + 1;
            }

            void D3D12SwapChain::WaitForGpu()
            {
                auto queue = static_cast<D3D12Device*>(Renderer::GetDevice())->GetQueue();

                //Wait until the fence has been processed
                m_fence->SetEventOnCompletion(m_fenceValues[m_currentBuffer], m_fenceEvent);
                WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

                m_fenceValues[m_currentBuffer]++;
            }

            CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetRenderTargetView()
            {
                return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_renderTargetHeap->GetCPUDescriptorHandleForHeapStart(),
                    m_currentBuffer,
                    static_cast<D3D12Device*>(Renderer::GetDevice())->GetRTVHeapIncrement());
            }

            CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetDepthStencilView()
            {
                return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_depthStencilTargetHeap->GetCPUDescriptorHandleForHeapStart());
            }
            ID3D12CommandList * D3D12SwapChain::GetCommandList()
            {
                return m_commandList;
            }
        }
    }
}