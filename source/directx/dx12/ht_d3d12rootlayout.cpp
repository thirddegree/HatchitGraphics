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
			}

			D3D12RootLayout::~D3D12RootLayout()
			{
				ReleaseCOM(m_rootSignature);
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
                for (int i = 0; i < resource->GetParameterCount(); i++)
                {
                    D3D12_ROOT_PARAMETER  parameter = {};
                    RootLayout::Parameter p = parameters[i];

                    switch (p.type)
                    {
                    case RootLayout::Parameter::Type::TABLE:
                    {
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
                        for (int j = 0; j < table.NumDescriptorRanges; j++)
                        {
                            D3D12_DESCRIPTOR_RANGE _range;
                            RootLayout::Range r = p.data.table.ranges[j];
                            _range.NumDescriptors = r.numDescriptors;
                            switch (r.type)
                            {
                            case RootLayout::Range::Type::CONSTANT_BUFFER:
                            {
                                _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                            } break;

                            case RootLayout::Range::Type::SAMPLER:
                            {
                                _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                            } break;

                            case RootLayout::Range::Type::SHADER_RESOURCE:
                            {
                                _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                            } break;

                            case RootLayout::Range::Type::UNORDERED_ACCESS:
                            {
                                _range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
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
                    } break;
                    }

                    _RootParameters.push_back(parameter);
                }

                /*Create Static Samplers*/


                rootDesc.NumParameters = resource->GetParameterCount();
                rootDesc.pParameters = &_RootParameters[0];
                rootDesc.NumStaticSamplers = 0;
                rootDesc.pStaticSamplers = nullptr;
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
                ReleaseCOM(_signature);

                hr = device->CreateRootSignature(0, _signature->GetBufferPointer(), _signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("Failed to create root signature.\n");
                    ReleaseCOM(_signature);
                    return false;
                }
                DX::RegisterDebugName(m_rootSignature, (wchar_t*)resource.GetName().c_str());
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

            std::vector<D3D12_STATIC_SAMPLER_DESC> D3D12RootLayout::SamplerDescsFromHandle(const Resource::RootLayoutHandle& handle)
            {
                using namespace Resource;

                std::vector<D3D12_STATIC_SAMPLER_DESC>  _descs;
                std::vector<Sampler> _RootSamplers = handle->GetSamplers();
                for (auto sampler : _RootSamplers)
                {
                    D3D12_STATIC_SAMPLER_DESC desc;
                    
                }

                return _descs;
            }
		}
	}
}