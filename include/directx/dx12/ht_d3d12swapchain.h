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
                D3D12SwapChain();

                bool VInitialize()  override;

                void VResize(uint32_t width, uint32_t height) override;

                void VPresent()     override;

            private:
                IDXGISwapChain3*        m_chain;
                ID3D12CommandAllocator* m_allocators;
                ID3D12Resource*         m_renderTargets[NUM_BUFFER_FRAMES];
                ID3D12Resource*         m_depthStencilTarget;
                ID3D12DescriptorHeap*   m_renderTargetHeap;
                ID3D12DescriptorHeap*   m_despthStencilTargetHeap;
                ID3D12CommandList*      m_commandList;
            };
        }
    }
}