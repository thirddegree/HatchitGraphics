/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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
#include <ht_texture_base.h>
#include <ht_texture_resource.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class D3D12Device;

            class HT_API D3D12Texture : public TextureBase
            {
            public:
                D3D12Texture();

                ~D3D12Texture();

                void Upload(ID3D12CommandList* commandList, uint32_t descriptorOffset);


            private:
                bool Initialize(Resource::TextureHandle handle, D3D12Device* _device);

                D3D12_RESOURCE_DESC         m_desc;
                ID3D12Resource*             m_texture;
                ID3D12Resource*             m_uploadHeap;

                friend class D3D12GPUResourceThread;
            };
        }
    }
}