/**
**    Hatchit Engine
**    Copyright(c) 2015-2016ThirdDegree
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
#include <ht_pipeline_base.h>
#include <ht_pipeline_resource.h>
#include <ht_shader.h>
#include <ht_shadervariablechunk.h>
#include <ht_directx.h>
#include <ht_d3d12shader.h>
#include <ht_refcounted.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            class HT_API D3D12Pipeline : public PipelineBase
            {
            public:
                D3D12Pipeline();

                ~D3D12Pipeline();

                ID3D12PipelineState* GetPipeline();

                bool Initialize(Resource::PipelineHandle handle, ID3D12Device* device);

                virtual bool VInitialize(const Resource::PipelineHandle handle);
                virtual bool VUpdate() override;

                virtual bool VSetShaderVariables(ShaderVariableChunk* variables) override;

                virtual bool VSetInt(size_t offset, int data) override;
                virtual bool VSetDouble(size_t offset, double data) override;
                virtual bool VSetFloat(size_t offset, float data) override;
                virtual bool VSetFloat2(size_t offset, Math::Vector2 data) override;
                virtual bool VSetFloat3(size_t offset, Math::Vector3 data) override;
                virtual bool VSetFloat4(size_t offset, Math::Vector4 data) override;
                virtual bool VSetMatrix4(size_t offset, Math::Matrix4 data) override;

            private:
                D3D12_GRAPHICS_PIPELINE_STATE_DESC m_description;
                ID3D12PipelineState*               m_pipelineState;

                std::map<Resource::Pipeline::ShaderSlot, ShaderHandle> m_shaders;

                D3D12_RASTERIZER_DESC   RasterDescFromHandle(const Resource::PipelineHandle& handle);
                D3D12_SHADER_BYTECODE   ShaderBytecodeFromHandle(Resource::Pipeline::ShaderSlot slot, const Resource::PipelineHandle& handle);
                D3D12_INPUT_LAYOUT_DESC InputLayoutDescFromHandle(const Resource::PipelineHandle& handle);
                DXGI_FORMAT             InputFormatFromElement(const Resource::ShaderVariable::Type& element);
            };

        }
    }
}