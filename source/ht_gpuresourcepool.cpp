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


        /**
        *   \fn GPUResourcePool::Initialize()
        *   \brief Initializes the GPUResourcePool singleton class
        *   \param device The GPU device object
        *   \param swapchain The current swapchain
        *
        *   Initializes the GPUResourcePool instance with the device and swapchain.
        *   
        *   NOTE:
        *   This is subject to change, as the GPUResourcePool shouldn't need to
        *   know about any specific swapchain object. In an application, there may be
        *   multiple renderers each with its own swapchain. Each of these renderers should
        *   use the same GPUResourcePool and share memory.
        *   
        */  
        bool GPUResourcePool::Initialize(IDevice* device, SwapChain* swapchain)
        {
            if (!device)
                return false;

            GPUResourcePool& instance = GPUResourcePool::instance();

            RendererType rendererType = Renderer::GetType();
            
            switch (rendererType)
            {
                case RendererType::DIRECTX12:
                {
#ifdef DX12_SUPPORT
                    instance.m_thread = new DX::D3D12GPUResourceThread(static_cast<DX::D3D12Device*>(device));
                    break;
#else
                    return false;
#endif
                }
                

                case RendererType::VULKAN:
                {
#ifdef VK_SUPPORT
                    instance.m_thread = new Vulkan::VKGPUResourceThread(static_cast<Vulkan::VKDevice*>(device), static_cast<Vulkan::VKSwapChain*>(swapchain));
                    break;
#else
                    return false;
#endif
                }

                case RendererType::DIRECTX11:
                case RendererType::OPENGL:
                case RendererType::UNKNOWN:
                    return false;

            }


            instance.m_device = device;
            
            return true;
        }
       
        /**
        *   \fn GPUResourcePool::DeInitialize()
        *   \brief Destroys and released the memory used by the GPUResourcePool
        *
        *   This function releases the memory used by the GPUResourcePool
        *   since it is a singleton instance and must be released safely by the application.
        */ 
        void GPUResourcePool::DeInitialize()
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            delete instance.m_thread;
        }

        /**
        *   \fn GPUResourcePool::IsLocked()
        *   \brief Returns true if the GPUResourcePool is locked, otherwise returns false.
        *
        *   Due to the multithreaded nature of the GPUResourcePool, it is necessary to know
        *   if the GPUResourcePool master thread is currently locked. This function returns true
        *   if the thread is locked, otherwise it returns false;
        */
        bool GPUResourcePool::IsLocked()
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            return instance.m_thread->Locked();
        }

        /**
        *   \fn GPUResourcePool::RequestTexture()
        *   \brief Function requests the GPUResourcePool to process a non-async texture load request.
        *   \param file Path of the texture file to load
        *   \param data Pointer to texture base implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous texture
        *   load request. Calling this function will block the main thread until the requested texture
        *   is loaded.
        */  
        void GPUResourcePool::RequestTexture(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            TextureRequest* request = new TextureRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Texture;
            request->data = data;

            instance.m_thread->Load(request);
        }

        /**
        *   \fn GPUResourcePool::RequestMaterial()
        *   \brief Function requests the GPUResourcePool to process a non-async material load request.
        *   \param file Path of the material file to load.
        *   \param data Pointer to the material base implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous material
        *   load request. Calling this function will block the main thread until the requested material
        *   is loaded.
        */
        void GPUResourcePool::RequestMaterial(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MaterialRequest* request = new MaterialRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Material;
            request->data = data;

            instance.m_thread->Load(request);
        }

        /**
        *   \fn GPUResourcePool::RequestRootLayout()    
        *   \brief Function requests the GPUResourcePool to process a non-async rootlayout load request.
        *   \param file Path of the rootlayout file to load.
        *   \param data Pointer to the rootlayout base implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous rootlayout
        *   load request. Calling this function will block the main thread until the requested rootlayout
        *   is loaded.
        */
        void GPUResourcePool::RequestRootLayout(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();
            
            RootLayoutRequest* request = new RootLayoutRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RootLayout;
            request->data = data;

            instance.m_thread->Load(request);
        }

        /**
        *   \fn GPUResourcePool::RequestPipeline()
        *   \brief Function requests the GPUResourcePool to process a non-async pipeline load request.
        *   \param file Path of the pipeline file to load.
        *   \param data Pointer to the pipeline base implementation to fill.
        *
        *   This function requestts the GPUResourcePool to process a non-asynchronous  pipeline
        *   load request. Calling this function will block the main thread until the requested pipeline
        *   is loaded.
        */
        void GPUResourcePool::RequestPipeline(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            PipelineRequest* request = new PipelineRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Pipeline;
            request->data = data;

            instance.m_thread->Load(request);
        }

        /**
        *   \fn GPUResourcePool::RequestShader()
        *   \brief Function requests the GPUResourcePool to process a non-async shader load request.
        *   \param file Path of the shader file to load.
        *   \param data Pointer to the shader base implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous shader
        *   load request. Calling this function will block the main thread until the requested shader
        *   is loaded.
        */
        void GPUResourcePool::RequestShader(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            ShaderRequest* request = new ShaderRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Shader;
            request->data = data;

            instance.m_thread->Load(request);
        }

        /**
        *   \fn GPUResourcePool::RequestRenderPass()
        *   \brief Function requests the GPUResourcePool to process a non-async renderpass load request.
        *   \param file Path of the renderpass file to load.
        *   \param data Pointer to the renderpass base implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous renderpass
        *   load request. Calling this function will block the main thread until the requested renderpass
        *   is loaded.
        */
        void GPUResourcePool::RequestRenderPass(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderPassRequest* request = new RenderPassRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RenderPass;
            request->data = data;

            instance.m_thread->Load(request);
        }
        
        /**
        *   \fn GPUResourcePool::RequestRenderTarget()
        *   \brief Function requests the GPUResourcePool to process a non-async rendertarget load request.
        *   \param file Path of the rendertarget file to load.
        *   \param data Pointer to base rendertarget implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous rendertarget
        *   load request. Calling this function will block the main thread until the requested rendertarget
        *   is loaded.
        */
        void GPUResourcePool::RequestRenderTarget(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderTargetRequest* request = new RenderTargetRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RenderTarget;
            request->data = data;

            instance.m_thread->Load(request);
        }


        /**
        *   \fn GPUResourcePool::RequestMesh()
        *   \brief Function requests the GPUResourcePool to process a non-async mesh load request.
        *   \param file Path of the mesh file to load.
        *   \param data Pointer to the base mesh implementation to fill.
        *
        *   This function requests the GPUResourcePool to process a non-asynchronous mesh
        *   load request. Calling this function will block the main thread until the requested mesh
        *   is loaded.
        */  
        void GPUResourcePool::RequestMesh(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MeshRequest* request = new MeshRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Mesh;
            request->data = data;

            instance.m_thread->Load(request);
        }

        /**
        *   \fn GPUResourcePool::RequestTextureAsync()
        *   \brief Function requests the GPUResourcePool to process an async texture load request.
        *   \param file Path of texture file to load.
        *   \param data Pointer to the base texture implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous texture
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestTextureAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            TextureRequest* request = new TextureRequest;
            request->file = file;
            request->data = data;
            request->type = GPUResourceRequest::Type::Texture;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestMaterialAsync()
        *   \brief Function requests the GPUResourcePool to process an async material load request.
        *   \param file Path to material file to load.
        *   \param data Pointer to the base material implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous material
        *   load request. Calling this function will NOT block the main thread. 
        */
        void GPUResourcePool::RequestMaterialAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MaterialRequest* request = new MaterialRequest;
            request->file = file;
            request->data = data;
            request->type = GPUResourceRequest::Type::Material;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestRootLayoutAsync()
        *   \brief Function requests the GPUResourcePool to process an async rootlayout load request.
        *   \param file Path to the rootlayout file to load.
        *   \param data Pointer to the base rootlayout implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous rootlayout
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestRootLayoutAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RootLayoutRequest* request = new RootLayoutRequest;
            request->file = file;
            request->data = data;
            request->type = GPUResourceRequest::Type::RootLayout;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestPipelineAsync(()
        *   \brief Function requests the GPUResourcePool to process an async pipeline load request.
        *   \param file Path to the pipeline file to load.
        *   \param data Pointer to the base pipeline implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous pipeline
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestPipelineAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            PipelineRequest* request = new PipelineRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Pipeline;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestShaderAsync()
        *   \brief Function requests the GPUResourcePool to process an async shader load request.
        *   \param file Path to the shader file to load.
        *   \param data Pointer to the base shader implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous shader
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestShaderAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            ShaderRequest* request = new ShaderRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Shader;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestRenderPassAsync()
        *   \brief Function requests the GPUResourcePool to process an async renderpass load request.
        *   \param file Path to renderpass file to load.
        *   \param data Pointer to the base renderpass implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous shader
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestRenderPassAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderPassRequest* request = new RenderPassRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RenderPass;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestRenderTargetAsync()
        *   \brief Function requests the GPUResourcePool to process an async rendertarget load request.
        *   \param file Path to the renderpass file to load.
        *   \param data Pointer to the base rendertarget implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous rendertarget
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestRenderTargetAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            RenderTargetRequest* request = new RenderTargetRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::RenderTarget;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::RequestMeshAsync()
        *   \brief Function requests the GPUResourcePool to process an async mesh load request.
        *   \param file Path to the mesh file to load
        *   \param data Pointer to the base mesh implementation to fill.
        *
        *   This function requests the GPUResourcePool to process an asynchronous mesh
        *   load request. Calling this function will NOT block the main thread.
        */
        void GPUResourcePool::RequestMeshAsync(std::string file, void** data)
        {
            GPUResourcePool& instance = GPUResourcePool::instance();

            MeshRequest* request = new MeshRequest;
            request->file = file;
            request->type = GPUResourceRequest::Type::Mesh;
            request->data = data;

            instance.m_thread->LoadAsync(request);
        }

        /**
        *   \fn GPUResourcePool::CreateTexture()
        *   \brief Function creates a texture within the GPUResourcePool thread.
        *   \param file Path to the texture file to load.
        *   \param data Pointer to the base texture implementation to fill.
        *
        *   This function creates a texture immediately on the GPUResourcePool.
        *   Generally this function should not be called, but rather you should instead
        *   use the RequestTexture or RequestTextureAsync functions.
        *
        *   The purpose of this function is to allow immediate creation of a texture if
        *   we find ourselves already within the resource thread, and we know it is safe to do so.
        *
        *   NOTE: See ht_texture.cpp for example usage.
        */
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
