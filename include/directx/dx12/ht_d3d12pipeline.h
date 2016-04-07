/**
**    Hatchit Engine
**    Copyright(c) 2015 ThirdDegree
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
#include <ht_pipeline.h>
#include <ht_directx.h>
#include <ht_d3d12shader.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            class HT_API D3D12Pipeline : public IPipeline
            {
            public:
                D3D12Pipeline(ID3D12Device* device, ID3D12RootSignature* rootSignature);

                // Inherited via IPipeline
                virtual bool VInitialize(const Resource::PipelineHandle handle) override;
                virtual void VSetRasterState(const Resource::Pipeline::RasterizerState & rasterState) override;
                virtual void VSetMultisampleState(const Resource::Pipeline::MultisampleState & multiState) override;
                virtual void VLoadShader(Resource::Pipeline::ShaderSlot shaderSlot, IShaderHandle shader) override;
                virtual bool VPrepare() override;

            private:
                D3D12_GRAPHICS_PIPELINE_STATE_DESC m_description;
                ID3D12Device*                      m_device;
                ID3D12PipelineState*               m_pipelineState;
                ID3D12RootSignature*               m_rootSignature;
                D3D12ShaderHandle                  m_shaders[Resource::Pipeline::MAX_SHADERS];


                D3D12_RASTERIZER_DESC   RasterDescFromHandle(const Resource::PipelineHandle& handle);
                D3D12_SHADER_BYTECODE   ShaderBytecodeFromHandle(Resource::Pipeline::ShaderSlot slot, const Resource::PipelineHandle& handle);
                D3D12_INPUT_LAYOUT_DESC InputLayoutDescFromHandle(const Resource::PipelineHandle& handle);
                DXGI_FORMAT             InputFormatFromElement(const Resource::Pipeline::InputElement& element);
            };
        }
    }
}