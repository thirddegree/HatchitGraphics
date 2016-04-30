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

#include <ht_gpuresourcepool.h>
#include <ht_gpuresourcethread.h>

#ifdef VK_SUPPORT
//#include <ht_vkgpuresourcethread.h>
#endif

#ifdef DX12_SUPPORT
#include <ht_d3d12gpuresourcethread.h>
#include <ht_d3d12device.h>
#endif

#include <ht_d3d12texture.h>

namespace Hatchit
{

    using namespace Core;

    namespace Graphics
    {
        using namespace DX;

        TextureHandle GPUResourcePool::_DefaultTexture;

        bool GPUResourcePool::Initialize(IDevice* device)
        {
            if (!device)
                return false;

            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread = new DX::D3D12GPUResourceThread(static_cast<DX::D3D12Device*>(device));
            instance.m_device = device;

            /*Initialize default assets*/
            _DefaultTexture = Texture::GetHandle("raptor.png", "raptor.png");

            return true;
        }
        
        void GPUResourcePool::DeInitialize()
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            delete instance.m_thread;

            //Release the static defaults
            _DefaultTexture.Release();
        }

        TextureHandle GPUResourcePool::RequestTexture(std::string file)
        {
            return Texture::GetHandle(file, file);
        }

        TextureHandle GPUResourcePool::RequestTextureAsync(std::string file) 
        {
            return Texture::GetHandleAsync(file, file);
        }

        MaterialHandle GPUResourcePool::RequestMaterial(std::string file) {}

        MaterialHandle GPUResourcePool::RequestMaterialAsync(std::string file) {}

        PipelineHandle GPUResourcePool::RequestPipeline(std::string file)
        {
        }
    }
}