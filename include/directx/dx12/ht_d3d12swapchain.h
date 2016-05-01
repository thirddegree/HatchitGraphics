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
#include <ht_directx.h>
#include <ht_swapchain.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12SwapChain : public SwapChain
            {
                static const uint32_t NUM_BUFFER_FRAMES = 2;
            public:
                D3D12SwapChain(HWND hwnd);

                ~D3D12SwapChain();
                
                void VClear(float* color) override;

                bool VInitialize(uint32_t width, uint32_t height)  override;

                void VResize(uint32_t width, uint32_t height) override;

                void VPresent()     override;

                CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView();

                CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView();

                ID3D12CommandList* GetCommandList();

            private:
                IDXGISwapChain3*                m_chain;
                ID3D12CommandAllocator*         m_commandAllocators[NUM_BUFFER_FRAMES];
                ID3D12Resource*                 m_renderTargets[NUM_BUFFER_FRAMES];
                ID3D12Resource*                 m_depthStencilTarget;
                ID3D12DescriptorHeap*           m_renderTargetHeap;
                ID3D12DescriptorHeap*           m_depthStencilTargetHeap;
                ID3D12GraphicsCommandList*      m_commandList;
                HWND                            m_hwnd;

                //CPU-GPU ynchronization
                ID3D12Fence*                m_fence;
                HANDLE                      m_fenceEvent;
                uint64_t                    m_fenceValues[NUM_BUFFER_FRAMES];
                uint64_t                    m_currentFence;

                bool CreateBuffers(uint32_t width, uint32_t height);
                void MoveToNextFrame();
                void WaitForGpu();
            };
        }
    }
}