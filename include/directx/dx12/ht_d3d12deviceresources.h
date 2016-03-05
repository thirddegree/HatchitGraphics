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
#include <ht_directx.h>

namespace Hatchit {
    namespace Graphics {

        namespace DX
        {
            class HT_API D3D12DeviceResources
            {
                static const int NUM_RENDER_TARGETS = 2;
                static const int NUM_BUFFER_FRAMES = 2;
            public:
                D3D12DeviceResources();

                ~D3D12DeviceResources();

                void Present();
                void WaitForGPU();
                void MoveToNextFrame();
                void ValidateDevice();
                bool Initialize(HWND hwnd, uint32_t width, uint32_t height);
            
                ID3D12Device*           GetDevice();
                IDXGISwapChain3*        GetSwapChain();
                ID3D12Resource*         GetRenderTarget();
                ID3D12Resource*         GetDepthStencil();
                ID3D12CommandAllocator* GetCommandAllocator();
                ID3D12CommandQueue*     GetCommandQueue();
                D3D12_VIEWPORT          GetScreenViewport();
                uint32_t                GetCurrentFrameIndex();

                CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView();
                CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView();
            private:
                bool                        m_deviceRemoved;
                uint32_t                    m_currentFrame;
                HANDLE                      m_fenceEvent;
                ID3D12Fence*                m_fence;
                uint64_t                    m_fenceValues[NUM_BUFFER_FRAMES];

                D3D12_VIEWPORT              m_viewport;
                D3D12_RECT                  m_scissorRect;
                ID3D12Device*               m_device;
                IDXGISwapChain3*            m_swapChain;
                ID3D12CommandQueue*         m_commandQueue;
                ID3D12CommandAllocator*     m_commandAllocator;
                ID3D12Resource*             m_renderTargets[NUM_RENDER_TARGETS];
                ID3D12Resource*             m_depthStencil;
                ID3D12DescriptorHeap*       m_renderTargetViewHeap;
                uint32_t                    m_renderTargetViewHeapSize;
                ID3D12DescriptorHeap*       m_depthStencilHeap;

            private:
                bool        CreateDeviceResources(HWND hwnd, uint32_t width, uint32_t height);
                HRESULT     CheckHardwareAdapter(IDXGIFactory2 * pFactory, IDXGIAdapter1 ** ppAdapter);
            };
        }
    }
}