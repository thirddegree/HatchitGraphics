/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 ThirdDegree
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

#include <ht_d3d12rootlayout.h>
#include <ht_debug.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12RootLayout::D3D12RootLayout(std::string ID)
                : Core::RefCounted<D3D12RootLayout>(std::move(ID))
            {
                m_rootSignature = nullptr;
                m_CBV_SRV_UAV_Heap = nullptr;
                m_RTV_Heap = nullptr;
                m_DSV_Heap = nullptr;

                m_totalCBV_SRV_UAV_Descriptors = 0;
            }

            D3D12RootLayout::~D3D12RootLayout()
            {
                ReleaseCOM(m_rootSignature);
                ReleaseCOM(m_CBV_SRV_UAV_Heap);
            }

            ID3D12DescriptorHeap* D3D12RootLayout::GetHeap(HeapType type)
            {
                switch (type)
                {
                case HeapType::CBV_SRV_UAV:
                    return m_CBV_SRV_UAV_Heap;
                case HeapType::DSV:
                    return m_DSV_Heap;
                case HeapType::RTV:
                    return m_RTV_Heap;
                }
                
                return nullptr;
            }

            bool D3D12RootLayout::Initialize(const std::string& fileName, ID3D12Device* device)
            {
                using namespace Resource;

                HRESULT hr = S_OK;

                Resource::RootLayoutHandle resource = Resource::RootLayout::GetHandleFromFileName(fileName);
                if (!resource.IsValid())
                    return false;

                D3D12_ROOT_SIGNATURE_DESC rootDesc;

                /*Build RootParameter Collection*/
                std::vector<D3D12_ROOT_PARAMETER> _RootParameters;
                std::vector<RootLayout::Parameter> parameters = resource->GetParameters();
                for (uint32_t i = 0; i < resource->GetParameterCount(); i++)
                {
                    
                    RootLayout::Parameter p = parameters[i];

                    switch (p.type)
                    {
                        case RootLayout::Parameter::Type::TABLE:
                        {
                            D3D12_ROOT_PARAMETER  parameter = {};
                            parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                            switch (p.visibility)
                            {
                                case RootLayout::ShaderVisibility::VERTEX:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
                                } break;

                                case RootLayout::ShaderVisibility::TESS_CONTROL:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
                                } break;

                                case RootLayout::ShaderVisibility::TESS_EVAL:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
                                } break;

                                case RootLayout::ShaderVisibility::GEOMETRY:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
                                } break;

                                case RootLayout::ShaderVisibility::FRAGMENT:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                                } break;

                                case RootLayout::ShaderVisibility::ALL:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                                } break;
                            }

                            D3D12_ROOT_DESCRIPTOR_TABLE table;
                            table.NumDescriptorRanges = p.data.table.rangeCount;
                            D3D12_DESCRIPTOR_RANGE* _ranges = new D3D12_DESCRIPTOR_RANGE[table.NumDescriptorRanges];
                            m_allocatedRanges.push_back(_ranges);
                            for (uint32_t j = 0; j < table.NumDescriptorRanges; j++)
                            {
                                D3D12_DESCRIPTOR_RANGE _range;
                                RootLayout::Range r = p.data.table.ranges[j];
                                _range.NumDescriptors = r.numDescriptors;
                                switch (r.type)
                                {
                                case RootLayout::Range::Type::CONSTANT_BUFFER:
                                {
                                    _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                                    m_totalCBV_SRV_UAV_Descriptors++;
                                } break;

                                case RootLayout::Range::Type::SAMPLER:
                                {
                                    _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                                } break;

                                case RootLayout::Range::Type::SHADER_RESOURCE:
                                {
                                    _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                                    m_totalCBV_SRV_UAV_Descriptors++;
                                } break;

                                case RootLayout::Range::Type::UNORDERED_ACCESS:
                                {
                                    _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                                    m_totalCBV_SRV_UAV_Descriptors++;
                                } break;

                                default:
                                    break;
                                }
                                _range.BaseShaderRegister = r.baseRegister;
                                _range.RegisterSpace = r.registerSpace;
                                _range.OffsetInDescriptorsFromTableStart = 0;

                                _ranges[j] = _range;
                            }
                            table.pDescriptorRanges = _ranges;
                            parameter.DescriptorTable = table;

                            _RootParameters.push_back(parameter);
                        } break;

                        case RootLayout::Parameter::Type::CONSTANTS:
                        {
                            D3D12_ROOT_PARAMETER parameter = {};
                            switch (p.visibility)
                            {
                                case RootLayout::ShaderVisibility::VERTEX:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
                                } break;

                                case RootLayout::ShaderVisibility::TESS_CONTROL:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
                                } break;

                                case RootLayout::ShaderVisibility::TESS_EVAL:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
                                } break;

                                case RootLayout::ShaderVisibility::GEOMETRY:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
                                } break;

                                case RootLayout::ShaderVisibility::FRAGMENT:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                                } break;

                                case RootLayout::ShaderVisibility::ALL:
                                {
                                    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                                } break;
                            }
                            parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                            parameter.Constants.Num32BitValues = p.data.constant.valueCount;
                            parameter.Constants.ShaderRegister = p.data.constant.shaderRegister;
                            parameter.Constants.RegisterSpace = p.data.constant.registerSpace;
                            
                            _RootParameters.push_back(parameter);
                        } break;

                    }

                }

                /*Create Static Samplers*/
                std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers = SamplerDescsFromHandle(resource);
                rootDesc.NumParameters = resource->GetParameterCount();
                rootDesc.pParameters = &_RootParameters[0];
                rootDesc.NumStaticSamplers = static_cast<uint32_t>(resource->GetSamplers().size());
                rootDesc.pStaticSamplers = &_Samplers[0];
                rootDesc.Flags = RootSignatureFlagsFromHandle(resource);

                ID3DBlob* _signature = nullptr;
                ID3DBlob* _error = nullptr;
                hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &_signature, &_error);
                if (FAILED(hr))
                {
                    if (_error)
                    {
                        HT_DEBUG_PRINTF("Failed to serialize root signature: %s\n", (char*)_error->GetBufferPointer());
                    }
                    ReleaseCOM(_signature);
                    ReleaseCOM(_error);
                    return false;
                }

                hr = device->CreateRootSignature(0, _signature->GetBufferPointer(), _signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create root signature.\n");
                    ReleaseCOM(_signature);
                    return false;
                }

                BuildDescriptorHeaps(device);

                HT_D3D12_DEBUGNAME(m_rootSignature, ("D3D12RootLayout [" + fileName + "]").c_str());
                PostInitCleanup();

                return true;
            }

            ID3D12RootSignature* D3D12RootLayout::GetRootSignature()
            {
                return m_rootSignature;
            }

            void D3D12RootLayout::PostInitCleanup()
            {
                /*Cleanup any allocated ranges*/
                for (auto range : m_allocatedRanges)
                    delete[] range;
            }

            bool D3D12RootLayout::BuildDescriptorHeaps(ID3D12Device* device)
            {
                HRESULT hr = S_OK;

                /*Create CBV_SRV_UAV Descriptor Heap*/
                D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
                heapDesc.NumDescriptors = m_totalCBV_SRV_UAV_Descriptors;
                heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                heapDesc.NodeMask = 0;
                hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_CBV_SRV_UAV_Heap));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create constant buffer descriptor heap.\n");
                    return false;
                }
                HT_D3D12_DEBUGNAME(m_CBV_SRV_UAV_Heap, "D3D12RootLayout [CBV_SRV_UAV Heap]");

                return true;
            }
            
            D3D12_ROOT_SIGNATURE_FLAGS D3D12RootLayout::RootSignatureFlagsFromHandle(const Resource::RootLayoutHandle& handle)
            {
                using namespace Resource;

                D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

                if (handle->GetDescriptorFlags().TestFlag(RootLayout::LAYOUT_ALLOW_INPUT_ASSEMLBER_INPUT_LAYOUT))
                    rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
                if (handle->GetDescriptorFlags().TestFlag(RootLayout::LAYOUT_DENY_VERTEX_SHADER_ROOT_ACCESS))
                    rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
                if (handle->GetDescriptorFlags().TestFlag(RootLayout::LAYOUT_DENY_TESS_CONTROL_SHADER_ROOT_ACCESS))
                    rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
                if (handle->GetDescriptorFlags().TestFlag(RootLayout::LAYOUT_DENY_TESS_EVAL_SHADER_ROOT_ACCESS))
                    rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
                if (handle->GetDescriptorFlags().TestFlag(RootLayout::LAYOUT_DENY_FRAGMENT_SHADER_ROOT_ACCESS))
                    rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

                return rootSignatureFlags;
            }

        

            D3D12_COMPARISON_FUNC D3D12RootLayout::CompareOpFromType(const Resource::Sampler::CompareOperation op)
            {
                using namespace Resource;

                switch (op)
                {
                case Sampler::CompareOperation::COMPARE_OP_ALWAYS:
                    return D3D12_COMPARISON_FUNC_ALWAYS;
                case Sampler::CompareOperation::COMPARE_OP_EQUAL:
                    return D3D12_COMPARISON_FUNC_EQUAL;
                case Sampler::CompareOperation::COMPARE_OP_GREATER:
                    return D3D12_COMPARISON_FUNC_GREATER;
                case Sampler::CompareOperation::COMPARE_OP_GREATER_EQUAL:
                    return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
                case Sampler::CompareOperation::COMPARE_OP_LESS:
                    return D3D12_COMPARISON_FUNC_LESS;
                case Sampler::CompareOperation::COMPARE_OP_LESS_EQUAL:
                    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
                case Sampler::CompareOperation::COMPARE_OP_NEVER:
                    return D3D12_COMPARISON_FUNC_NEVER;
                case Sampler::CompareOperation::COMPARE_OP_NOT_EQUAL:
                    return D3D12_COMPARISON_FUNC_NOT_EQUAL;
                default:
                    return D3D12_COMPARISON_FUNC_NEVER;
                }
            }

            D3D12_STATIC_BORDER_COLOR D3D12RootLayout::BorderColorFromType(const Resource::Sampler::BorderColor color)
            {
                using namespace Resource;

                switch (color)
                {
                case Sampler::BorderColor::COLOR_OPAQUE_WHITE:
                    return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
                case Sampler::BorderColor::COLOR_OPAQUE_BLACK:
                    return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
                case Sampler::BorderColor::COLOR_TRANSPARENT_BLACK:
                    return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                default:
                    return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
                }
            }

            D3D12_FILTER D3D12RootLayout::FilterFromType(const Resource::Sampler::Filter& filter)
            {
                using namespace Resource;

                if (filter.mag == Sampler::FilterMode::NEAREST &&
                    filter.min == Sampler::FilterMode::NEAREST)
                    return D3D12_FILTER_MIN_MAG_MIP_POINT;
                else if (filter.mag == Sampler::FilterMode::BILINEAR &&
                    filter.min == Sampler::FilterMode::BILINEAR)
                    return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                else
                    return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            }

            D3D12_TEXTURE_ADDRESS_MODE D3D12RootLayout::AddressModeFromType(const Resource::Sampler::AddressMode mode)
            {
                using namespace Resource;

                switch (mode)
                {
                case Sampler::AddressMode::BORDER:
                    return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                case Sampler::AddressMode::CLAMP:
                    return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                case Sampler::AddressMode::MIRROR:
                    return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
                case Sampler::AddressMode::MIRROR_ONCE:
                    return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
                case Sampler::AddressMode::WRAP:
                    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;

                default:
                    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                }
            }

            std::vector<D3D12_STATIC_SAMPLER_DESC> D3D12RootLayout::SamplerDescsFromHandle(const Resource::RootLayoutHandle& handle)
            {
                using namespace Resource;

                std::vector<D3D12_STATIC_SAMPLER_DESC>  _descs;
                std::vector<Sampler> _RootSamplers = handle->GetSamplers();
                for (auto sampler : _RootSamplers)
                {
                    D3D12_STATIC_SAMPLER_DESC desc;
                    desc.AddressU = AddressModeFromType(sampler.GetAddress().u);
                    desc.AddressV = AddressModeFromType(sampler.GetAddress().v);
                    desc.AddressW = AddressModeFromType(sampler.GetAddress().w);
                    desc.MaxAnisotropy = sampler.GetMaxAnisotropy();
                    desc.MaxLOD = sampler.GetMaxLOD();
                    desc.MinLOD = sampler.GetMinLOD();
                    desc.MipLODBias = sampler.GetMipLODBias();
                    desc.Filter = FilterFromType(sampler.GetFilter());
                    desc.BorderColor = BorderColorFromType(sampler.GetBorderColor());
                    desc.ComparisonFunc = CompareOpFromType(sampler.GetCompareOp());
                    desc.ShaderRegister = sampler.GetImmutable().shaderRegister;
                    desc.RegisterSpace = sampler.GetImmutable().registerSpace;
                    switch (sampler.GetImmutable().visibility)
                    {
                        case Sampler::ShaderVisibility::VERTEX:
                        {
                            desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
                        } break;

                        case Sampler::ShaderVisibility::TESS_CONTROL:
                        {
                            desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
                        } break;

                        case Sampler::ShaderVisibility::TESS_EVAL:
                        {
                            desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
                        } break;

                        case Sampler::ShaderVisibility::GEOMETRY:
                        {
                            desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
                        } break;

                        case Sampler::ShaderVisibility::FRAGMENT:
                        {
                            desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
                        } break;

                        case Sampler::ShaderVisibility::ALL:
                        {
                            desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                        } break;
                    }

                    _descs.push_back(desc);
                }

                return _descs;
            }
        }
    }
}