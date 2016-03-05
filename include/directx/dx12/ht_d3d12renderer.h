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
#include <ht_d3d12indexbuffer.h>
#include <ht_d3d12deviceresources.h>
#include <ht_math.h>
#include <DirectXMath.h>
namespace Hatchit {

    namespace Graphics {

        namespace DX {

            struct ConstantBuffer
            {
                /*Math::Matrix4 world;
                Math::Matrix4 view;
                Math::Matrix4 proj;*/
                DirectX::XMFLOAT4X4 world;
                DirectX::XMFLOAT4X4 view;
                DirectX::XMFLOAT4X4 proj;
            };

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

                void VRender()                                          override;

            private:
                D3D12DeviceResources*       m_resources;
                ID3D12RootSignature*        m_rootSignature;
                ID3D12PipelineState*        m_pipelineState;
                ID3D12GraphicsCommandList*  m_commandList;
                ID3D12DescriptorHeap*       m_cbDescriptorHeap;
                uint32_t                    m_cbDescriptorSize;
                uint8_t*                    m_mappedConstantBuffer;
     
                Color                       m_clearColor;

                //Demo only
                float                       m_aspectRatio;
                ID3D12Resource*             m_vertexBuffer;
                ID3D12Resource*             m_indexBuffer;
                ID3D12Resource*             m_constantBuffer;
                ID3DBlob*                   m_vertexShader;
                ID3DBlob*                   m_pixelShader;
                D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView;
                D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;

                bool LoadShaderFiles();

                ConstantBuffer              m_constantBufferData;
                static const UINT c_alignedConstantBufferSize = (sizeof(ConstantBuffer) + 255) & ~255;

            };
        }
    }
}