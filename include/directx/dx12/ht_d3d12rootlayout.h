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
#include <ht_rootlayout.h>

namespace Hatchit
{
	namespace Graphics
	{
		namespace DX
		{
			class HT_API D3D12RootLayout : public IRootLayout
			{
			public:
				D3D12RootLayout(ID3D12Device* device);

				~D3D12RootLayout();

                bool VDeferredInitialize(Resource::RootLayoutHandle resource) override;

				bool VInitialize(const Resource::RootLayoutHandle handle) override;

				ID3D12RootSignature* GetRootSignature();

			private:
				ID3D12RootSignature*  m_rootSignature;
				ID3D12Device*		  m_device;

				std::vector<D3D12_DESCRIPTOR_RANGE*> m_allocatedRanges;

				void								        PostInitCleanup();
                std::vector<D3D12_STATIC_SAMPLER_DESC>      SamplerDescsFromHandle(const Resource::RootLayoutHandle& handle);
				D3D12_ROOT_SIGNATURE_FLAGS			        RootSignatureFlagsFromHandle(const Resource::RootLayoutHandle& handle);
			};
		}
	}
}