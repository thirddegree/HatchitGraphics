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

#include <ht_d3d12pipeline.h>
#include <ht_d3d12rootlayout.h>
#include <ht_rootlayout.h>
#include <ht_renderpass_resource.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            D3D12Pipeline::D3D12Pipeline()
            {
                m_pipelineState = nullptr;
            }

            D3D12Pipeline::~D3D12Pipeline()
            {
                ReleaseCOM(m_pipelineState);
                m_shaders.clear();
            }

            ID3D12PipelineState* D3D12Pipeline::GetPipeline()
            {
                return m_pipelineState;
            }

            bool D3D12Pipeline::Initialize(Resource::PipelineHandle handle, ID3D12Device * device)
            {
                HRESULT hr = S_OK;

                Resource::RenderPassHandle renderPass = Resource::RenderPass::GetHandle(handle->GetRenderPassPath(),
                    handle->GetRenderPassPath());

                m_description = {};

                /*Build Input Layout*/
                std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
                const std::vector<Resource::Pipeline::Attribute>& _vertexAttributes = handle->GetVertexLayout();
                for (int i = 0; i < _vertexAttributes.size(); i++)
                {
                    D3D12_INPUT_ELEMENT_DESC elementDesc;
                    elementDesc.SemanticName = _vertexAttributes[i].semanticName.c_str();
                    elementDesc.SemanticIndex = _vertexAttributes[i].semanticIndex;
                    elementDesc.InputSlot = _vertexAttributes[i].slot;
                    elementDesc.Format = InputFormatFromElement(_vertexAttributes[i].type);
                    elementDesc.AlignedByteOffset = (i == 0) ? 0 : D3D12_APPEND_ALIGNED_ELEMENT;
                    elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    elementDesc.InstanceDataStepRate = 0;

                    _elements.push_back(elementDesc);
                }

                D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
                inputLayoutDesc.pInputElementDescs = &_elements[0];
                inputLayoutDesc.NumElements = static_cast<uint32_t>(_elements.size());

                auto rootSignature = RootLayout::GetHandle(renderPass->GetRootLayoutPath(), renderPass->GetRootLayoutPath());

                /*Build Pipeline State Object*/
                m_description.InputLayout = inputLayoutDesc;
                m_description.pRootSignature = static_cast<D3D12RootLayout*>(rootSignature->GetBase())->GetRootSignature();
                m_description.VS = ShaderBytecodeFromHandle(Resource::Pipeline::VERTEX, handle);
                m_description.GS = ShaderBytecodeFromHandle(Resource::Pipeline::GEOMETRY, handle);
                m_description.HS = ShaderBytecodeFromHandle(Resource::Pipeline::TESS_CONTROL, handle);
                m_description.DS = ShaderBytecodeFromHandle(Resource::Pipeline::TESS_EVAL, handle);
                m_description.PS = ShaderBytecodeFromHandle(Resource::Pipeline::FRAGMENT, handle);
                m_description.RasterizerState = RasterDescFromHandle(handle);
                m_description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
                m_description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
                m_description.SampleMask = UINT_MAX;
                m_description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                m_description.NumRenderTargets = 1;
                m_description.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
                m_description.DSVFormat = DXGI_FORMAT_D32_FLOAT;
                m_description.SampleDesc.Count = 1;

                hr = device->CreateGraphicsPipelineState(&m_description, IID_PPV_ARGS(&m_pipelineState));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create pipeline state object.\n");
                    return false;
                }

                return true;
            }

            bool D3D12Pipeline::VInitialize(const Resource::PipelineHandle handle)
            {
                return false;
            }

            bool D3D12Pipeline::VUpdate()
            {
                return false;
            }

            bool D3D12Pipeline::VSetShaderVariables(ShaderVariableChunk* variables)
            {
                return false;
            }

            bool D3D12Pipeline::VSetInt(size_t offset, int data)
            {
                return false;
            }

            bool D3D12Pipeline::VSetDouble(size_t offset, double data)
            {
                return false;
            }

            bool D3D12Pipeline::VSetFloat(size_t offset, float data)
            {
                return false;
            }

            bool D3D12Pipeline::VSetFloat2(size_t offset, Math::Vector2 data)
            {
                return false;
            }

            bool D3D12Pipeline::VSetFloat3(size_t offset, Math::Vector3 data)
            {
                return false;
            }

            bool D3D12Pipeline::VSetFloat4(size_t offset, Math::Vector4 data)
            {
                return false;
            }

            bool D3D12Pipeline::VSetMatrix4(size_t offset, Math::Matrix4 data)
            {
                return false;
            }

            D3D12_RASTERIZER_DESC D3D12Pipeline::RasterDescFromHandle(const Resource::PipelineHandle& handle)
            {
                D3D12_RASTERIZER_DESC desc;
                Resource::Pipeline::RasterizerState rasterState = handle->GetRasterizationState();

                //Handle cullmode
                switch (rasterState.cullMode)
                {
                case Resource::Pipeline::CullMode::BACK:
                    desc.CullMode = D3D12_CULL_MODE_BACK;
                    break;
                case Resource::Pipeline::CullMode::FRONT:
                    desc.CullMode = D3D12_CULL_MODE_FRONT;
                    break;
                case Resource::Pipeline::CullMode::NONE:
                    desc.CullMode = D3D12_CULL_MODE_NONE;
                    break;
                }

                //Handle polygon mode
                switch (rasterState.polygonMode)
                {
                case Resource::Pipeline::PolygonMode::SOLID:
                    desc.FillMode = D3D12_FILL_MODE_SOLID;
                    break;
                case Resource::Pipeline::PolygonMode::LINE:
                    desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
                    break;
                }

                desc.DepthClipEnable = rasterState.depthClampEnable;
                desc.FrontCounterClockwise = rasterState.frontCounterClockwise;
                desc.DepthBias = 0;
                desc.DepthBiasClamp = 0.0f;
                desc.SlopeScaledDepthBias = 0.0f;
                desc.MultisampleEnable = false;
                desc.AntialiasedLineEnable = false;
                desc.ForcedSampleCount = 0;
                desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

                return desc;
            }

            D3D12_SHADER_BYTECODE D3D12Pipeline::ShaderBytecodeFromHandle(Resource::Pipeline::ShaderSlot slot, const Resource::PipelineHandle & handle)
            {
                auto handles = handle->GetCSOShaderHandles();
                auto paths = handle->GetCSOShaderPaths();

                if (handles[slot].IsValid())
                {
                    ShaderHandle _handle = Shader::GetHandle(paths[slot], paths[slot]);
                    if (_handle.IsValid())
                    {
                        return static_cast<D3D12Shader*>(_handle->GetBase())->GetBytecode();
                    }
                    else
                    {
                        return CD3DX12_SHADER_BYTECODE(nullptr, 0);
                    }
                }
                else
                {
                    return CD3DX12_SHADER_BYTECODE(nullptr, 0);
                }
            }

            D3D12_INPUT_LAYOUT_DESC D3D12Pipeline::InputLayoutDescFromHandle(const Resource::PipelineHandle& handle)
            {
                std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
                const std::vector<Resource::Pipeline::Attribute>& _vertexAttributes = handle->GetVertexLayout();
                for (int i = 0; i < _vertexAttributes.size(); i++)
                {
                    Resource::Pipeline::Attribute element = _vertexAttributes[i];

                    D3D12_INPUT_ELEMENT_DESC elementDesc;
                    elementDesc.SemanticName = element.semanticName.c_str();
                    elementDesc.SemanticIndex = element.semanticIndex;
                    elementDesc.InputSlot = element.slot;
                    elementDesc.Format = InputFormatFromElement(element.type);
                    elementDesc.AlignedByteOffset = (i == 0) ? 0 : D3D12_APPEND_ALIGNED_ELEMENT;
                    elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    elementDesc.InstanceDataStepRate = 0;

                    _elements.push_back(elementDesc);
                }

                D3D12_INPUT_LAYOUT_DESC desc;
                desc.pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[3];
                return D3D12_INPUT_LAYOUT_DESC();
            }

            DXGI_FORMAT D3D12Pipeline::InputFormatFromElement(const Resource::ShaderVariable::Type& element)
            {
                switch (element)
                {
                case Resource::ShaderVariable::Type::FLOAT2:
                    return DXGI_FORMAT_R32G32_FLOAT;
                case Resource::ShaderVariable::Type::FLOAT3:
                    return DXGI_FORMAT_R32G32B32_FLOAT;
                case Resource::ShaderVariable::Type::FLOAT4:
                    return DXGI_FORMAT_R32G32B32A32_FLOAT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
                }
            }
        }
    }
}