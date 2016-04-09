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

#include <ht_d3d12rendertarget.h>

namespace Hatchit
{
	namespace Graphics
	{
		namespace DX
		{
			D3D12RenderTarget::D3D12RenderTarget()
			{

			}

            bool D3D12RenderTarget::VDeferredInitialize(Resource::RenderTargetHandle resource)
            {
                return true;
            }

			bool D3D12RenderTarget::Initialize(Resource::RenderTargetHandle handle, ID3D12Device* device)
			{
                

				return true;
			}
		}
	}
}