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
			D3D12RootLayout::D3D12RootLayout(ID3D12Device* device)
			{
				m_device = device;
				m_rootSignature = nullptr;
			}

			D3D12RootLayout::~D3D12RootLayout()
			{
				ReleaseCOM(m_rootSignature);
			}

			ID3D12RootSignature* D3D12RootLayout::GetRootSignature()
			{
				return m_rootSignature;
			}

			bool D3D12RootLayout::VInitialize(const Resource::RootLayoutHandle handle)
			{
				using namespace Resource;

				HRESULT hr = S_OK;
				
				if (!handle.IsValid())
					return false;

				D3D12_ROOT_SIGNATURE_DESC rootDesc;

				/*Build RootParameter Collection*/
				std::vector<D3D12_ROOT_PARAMETER> _RootParameters;
				std::vector<RootLayout::Parameter> parameters = handle->GetParameters();
				for (int i = 0; i < handle->GetParameterCount(); i++)
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
								case RootLayout::Parameter::Visibility::VERTEX:
								{
									parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
								} break;

								case RootLayout::Parameter::Visibility::TESS_CONTROL:
								{
									parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
								} break;

								case RootLayout::Parameter::Visibility::TESS_EVAL:
								{
									parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
								} break;

								case RootLayout::Parameter::Visibility::GEOMETRY:
								{
									parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
								} break;

								case RootLayout::Parameter::Visibility::FRAGMENT:
								{
									parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
								} break;

								case RootLayout::Parameter::Visibility::ALL:
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

				rootDesc.NumParameters = handle->GetParameterCount();
				rootDesc.pParameters = &_RootParameters[0];
				rootDesc.NumStaticSamplers = 0;
				rootDesc.pStaticSamplers = nullptr;
				rootDesc.Flags = RootSignatureFlagsFromHandle(handle);

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

				hr = m_device->CreateRootSignature(0, _signature->GetBufferPointer(), _signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
				if (FAILED(hr))
				{
					HT_DEBUG_PRINTF("Failed to create root signature.\n");
					ReleaseCOM(_signature);
					return false;
				}
				
				return true;
			}

			void D3D12RootLayout::PostInitCleanup()
			{
				/*Cleanup any allocated ranges*/
				for (auto range : m_allocatedRanges)
					delete[] range;
			}
			
			D3D12_ROOT_SIGNATURE_FLAGS D3D12RootLayout::RootSignatureFlagsFromHandle(const Resource::RootLayoutHandle& handle)
			{
				D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

				return rootSignatureFlags;
			}
		}
	}
}