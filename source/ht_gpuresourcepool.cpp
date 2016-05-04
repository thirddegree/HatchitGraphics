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
#include <ht_gpuresourcerequest.h>

#ifdef VK_SUPPORT
#include <ht_vkgpuresourcethread.h>
#include <ht_vkdevice.h>
#endif

#ifdef DX12_SUPPORT
#include <ht_d3d12gpuresourcethread.h>
#include <ht_d3d12device.h>
#include <ht_d3d12texture.h>
#endif

#include <ht_renderer.h>
#include <ht_swapchain.h>

namespace Hatchit
{

    using namespace Core;

    namespace Graphics
    {
        using namespace DX;

        bool GPUResourcePool::Initialize(IDevice* device, SwapChain* swapchain)
        {
            if (!device)
                return false;

            GPUResourcePool& instance = GPUResourcePool::instance();

            RendererType rendererType = Renderer::GetType();
            
            switch (rendererType)
            {
#ifdef DX12_SUPPORT
                case RendererType::DIRECTX12:
                    instance.m_thread = new DX::D3D12GPUResourceThread(static_cast<DX::D3D12Device*>(device));
                    break;
#endif
#ifdef VK_SUPPORT
                case RendererType::VULKAN:
                    instance.m_thread = new Vulkan::VKGPUResourceThread(static_cast<Vulkan::VKDevice*>(device), static_cast<Vulkan::VKSwapChain*>(swapchain));
                    break;
#endif
            }


            instance.m_device = device;
            
            return true;
        }
        
        void GPUResourcePool::DeInitialize()
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            delete instance.m_thread;
        }

        bool GPUResourcePool::IsLocked()
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            return instance.m_thread->Locked();
        }

        void GPUResourcePool::RequestTexture(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            TextureRequest* request = new TextureRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Texture;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestMaterial(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MaterialRequest* request = new MaterialRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Material;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestRootLayout(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();
            
            RootLayoutRequest* request = new RootLayoutRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RootLayout;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestPipeline(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            PipelineRequest* request = new PipelineRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Pipeline;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestShader(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            ShaderRequest* request = new ShaderRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Shader;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestRenderPass(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderPassRequest* request = new RenderPassRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RenderPass;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestRenderTarget(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderTargetRequest* request = new RenderTargetRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RenderTarget;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestMesh(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MeshRequest* request = new MeshRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Mesh;
            request->data = data;

            instance.m_thread->Load(request);
        }

        void GPUResourcePool::RequestTextureAsync(TextureHandle _default, TextureHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            TextureRequest* request = new TextureRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->data = data;
            request->type = GPUResourceRequest::Type::Texture;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestMaterialAsync(MaterialHandle _default, MaterialHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MaterialRequest* request = new MaterialRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->data = data;
            request->type = GPUResourceRequest::Type::Material;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestRootLayoutAsync(RootLayoutHandle _default, RootLayoutHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RootLayoutRequest* request = new RootLayoutRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->data = data;
            request->type = GPUResourceRequest::Type::RootLayout;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestPipelineAsync(PipelineHandle _default, PipelineHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            PipelineRequest* request = new PipelineRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->type = GPUResourceRequest::Type::Pipeline;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestShaderAsync(ShaderHandle _default, ShaderHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            ShaderRequest* request = new ShaderRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->type = GPUResourceRequest::Type::Shader;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestRenderPassAsync(RenderPassHandle _default, RenderPassHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderPassRequest* request = new RenderPassRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->type = GPUResourceRequest::Type::RenderPass;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestRenderTargetAsync(RenderTargetHandle _default, RenderTargetHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderTargetRequest* request = new RenderTargetRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->type = GPUResourceRequest::Type::RenderTarget;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::RequestMeshAsync(MeshHandle _default, MeshHandle temporary, std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MeshRequest* request = new MeshRequest;
            request->file = file;
            request->defaultHandle = _default;
            request->tempHandle = temporary;
            request->type = GPUResourceRequest::Type::Mesh;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        void GPUResourcePool::CreateTexture(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateTexture(file, data);
        }

        void GPUResourcePool::CreateMaterial(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateMaterial(file, data);
        }

        void GPUResourcePool::CreateRootLayout(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateRootLayout(file, data);
        }
        
        void GPUResourcePool::CreatePipeline(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreatePipeline(file, data);
        }

        void GPUResourcePool::CreateShader(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateShader(file, data);
        }

        void GPUResourcePool::CreateRenderPass(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateRenderPass(file, data);
        }

        void GPUResourcePool::CreateRenderTarget(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateRenderTarget(file, data);
        }

        void GPUResourcePool::CreateMesh(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            instance.m_thread->CreateMesh(file, data);
        }
        
     
    }
}