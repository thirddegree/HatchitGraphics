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

#pragma once

#include <ht_platform.h>
#include <ht_directx.h>
#include <ht_rootlayout_base.h>
#include <ht_rootlayout_resource.h>
#include <ht_refcounted.h>
#include <ht_sampler_resource.h>

namespace Hatchit
{ 
    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12RootLayout : public RootLayoutBase
            {
            public:
                enum class HeapType
                {
                    RTV,
                    CBV_SRV_UAV,
                    DSV
                };

                D3D12RootLayout();

                ~D3D12RootLayout();

                bool Initialize(Resource::RootLayoutHandle handle, ID3D12Device* device);

                ID3D12RootSignature* GetRootSignature();

            private:
                ID3D12RootSignature*  m_rootSignature;

                std::vector<D3D12_DESCRIPTOR_RANGE*>        m_allocatedRanges;

                void                                        PostInitCleanup();
                std::vector<D3D12_STATIC_SAMPLER_DESC>      SamplerDescsFromHandle(const Resource::RootLayoutHandle& handle);
               
                D3D12_ROOT_SIGNATURE_FLAGS			        RootSignatureFlagsFromHandle(const Resource::RootLayoutHandle& handle);
            
                D3D12_COMPARISON_FUNC                       CompareOpFromType(const Resource::Sampler::CompareOperation op);
                D3D12_STATIC_BORDER_COLOR                   BorderColorFromType(const Resource::Sampler::BorderColor color);
                D3D12_FILTER                                FilterFromType(const Resource::Sampler::Filter& filter);
                D3D12_TEXTURE_ADDRESS_MODE                  AddressModeFromType(const Resource::Sampler::AddressMode mode);
            
            };
        }
    }
}