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
#include <ht_renderer.h>
#include <ht_d3d12vertexbuffer.h>

namespace Hatchit {

    namespace Graphics {

        namespace DirectX {

            class HT_API D3D12Renderer : public IRenderer
            {
                static const int NUM_RENDER_TARGETS = 2;
            public:
                D3D12Renderer();

                ~D3D12Renderer();

                bool VInitialize(const RendererParams& params)          override;

                void VDeInitialize()                                    override;

                void VResizeBuffers(uint32_t width, uint32_t height)    override;

                void VSetClearColor(const Color& color)                 override;

                void VClearBuffer(ClearArgs args)                       override;

                void VPresent()                                         override;

            private:
                D3D12_VIEWPORT              m_viewport;
                D3D12_RECT                  m_scissorRect;
                ID3D12Device*               m_device;
                IDXGISwapChain3*            m_swapChain;
                ID3D12CommandQueue*         m_commandQueue;
                ID3D12CommandAllocator*     m_commandAllocator;
                ID3D12GraphicsCommandList*  m_commandList;
                ID3D12RootSignature*        m_rootSignature;
                ID3D12PipelineState*        m_pipelineState;
                ID3D12Resource*             m_renderTargets[NUM_RENDER_TARGETS];
                ID3D12DescriptorHeap*       m_renderTargetViewHeap;
                uint32_t                    m_renderTargetViewHeapSize;
                Color                       m_clearColor;

                //Synchronization objects.
                uint32_t                    m_frameIndex;
                HANDLE                      m_fenceEvent;
                ID3D12Fence*                m_fence;
                uint64_t                    m_fenceValue;

                //Demo only
                float                       m_aspectRatio;
                ID3D12Resource*             m_vertexBuffer;
                D3D12VertexBuffer*          m_vBuffer;
                D3D12_VERTEX_BUFFER_VIEW    m_vertexBufferView;
                D3D12_INDEX_BUFFER_VIEW     m_indexBufferView;

            private:
                HRESULT checkHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
                void    waitForFrame();

                
                
            };
        }
    }
}