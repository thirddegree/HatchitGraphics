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
#include <ht_d3d12deviceresources.h>
#include <ht_rendertarget_resource.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12RenderTarget::D3D12RenderTarget()
            {

            }

            bool D3D12RenderTarget::Initialize(const std::string& fileName)
            {
                using namespace Resource;

                Resource::RenderTargetHandle handle = Resource::RenderTarget::GetHandleFromFileName(fileName);
                if (!handle.IsValid())
                {
                    return false;
                }

                std::string format = handle->GetFormat();
                
                /*Create Render Target*/

                HRESULT hr = S_OK;

                m_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                

                return true;
            }

            DXGI_FORMAT D3D12RenderTarget::TargetFormatFromString(std::string s)
            {
                if (s == "BGRA")
                {
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                }
                else if (s == "RGBA")
                {
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                }
                else if (s == "BGR")
                {
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                }
                else if (s == "RGB")
                {
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                }
                else if (s == "R")
                {
                    return DXGI_FORMAT_R32_FLOAT;
                }

                return DXGI_FORMAT_UNKNOWN;
            }
        }
    }
}