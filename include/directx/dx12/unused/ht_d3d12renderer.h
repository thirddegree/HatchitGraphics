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
#include <ht_d3d12pipeline.h>
#include <ht_d3d12texture.h>
#include <ht_d3d12constantbuffer.h>
#include <ht_math.h>
#include <DirectXMath.h>
namespace Hatchit {

    namespace Graphics {

        namespace DX {

            struct _MM_ALIGN16 ConstantBuffer
            {
                Math::Matrix4 world;
                Math::Matrix4 view;
                Math::Matrix4 proj;
            };

            class HT_API D3D12Renderer : public Graphics::Renderer
            {
                static const int NUM_BUFFER_FRAMES = 2;
            public:
                D3D12Renderer();

                ~D3D12Renderer();

                bool VInitialize(const RendererParams& params)          override;

                void VDeInitialize()                                    override;

                void VResizeBuffers(uint32_t width, uint32_t height)    override;

                void VSetClearColor(const Color& color)                 override;

                void VClearBuffer(ClearArgs args)                       override;

                void VPresent()                                         override;

                void VRender(float dt)                                  override;

            private:
                D3D12DeviceResources*       m_resources;
                uint32_t                    m_cbDescriptorSize;
                D3D12PipelineHandle         m_pipeline;
                Color                       m_clearColor;

                //Demo only
                float                       m_aspectRatio;
                D3D12ConstantBuffer*        m_cBuffer;

                D3D12VertexBuffer*          m_vBuffer;
                D3D12IndexBuffer*           m_iBuffer;
                size_t                      m_numIndices;
                ConstantBuffer              m_constantBufferData;

                void BuildMeshData();
            };
        }
    }
}