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

#include <ht_d3d12gpuresourcethread.h>
#include <ht_device.h>
#include <ht_d3d12device.h>
#include <ht_d3d12texture.h>
#include <ht_d3d12material.h>
#include <ht_d3d12rootlayout.h>
#include <ht_d3d12pipeline.h>
#include <ht_d3d12shader.h>
#include <ht_d3d12renderpass.h>
#include <ht_d3d12rendertarget.h>
//#include <ht_d3d12mesh.h>

#include <ht_debug.h>
#include <ht_gpuresourcerequest.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12GPUResourceThread::D3D12GPUResourceThread(D3D12Device* device)
            {
                m_alive = false;
                m_tfinished = false;
                m_device = device;
                m_locked = false;
            }

            D3D12GPUResourceThread::~D3D12GPUResourceThread()
            {
                Kill();
            }

            void D3D12GPUResourceThread::VStart()
            {
                m_alive = true;

                m_thread = std::thread(&D3D12GPUResourceThread::thread_main, this);
            }

            void D3D12GPUResourceThread::thread_main()
            {
                HRESULT hr = S_OK;

                auto device = m_device->GetDevice();
                auto queue = m_device->GetQueue();

                /*Create thread specific resources*/
                ID3D12CommandAllocator* _allocator = nullptr;
                ID3D12GraphicsCommandList* cmdList = nullptr;
                
                hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_allocator));
                if (FAILED(hr))
                {
                    HT_ERROR_PRINTF("Failed to create command allocator in thread.\n");
                    Kill();
                }

                while (m_alive)
                {
                    /*Load any resource requests asynchrounsly*/
                    if (m_requests.empty())
                        continue;
                   
                    auto request = m_requests.pop();
                    switch ((*request)->type)
                    {
                        case GPUResourceRequest::Type::Texture:
                        {
                            auto tRequest = static_cast<TextureRequest*>(*request);

                            ProcessTextureRequest(tRequest);
                        } break;
                        
                        case GPUResourceRequest::Type::Material:
                        {
                            auto mRequest = static_cast<MaterialRequest*>(*request);

                            ProcessMaterialRequest(mRequest);
                        } break;

                        case GPUResourceRequest::Type::RootLayout:
                        {
                            auto rRequest = static_cast<RootLayoutRequest*>(*request);

                            ProcessRootLayoutRequest(rRequest);
                        } break;

                        case GPUResourceRequest::Type::Pipeline:
                        {
                            auto pRequest = static_cast<PipelineRequest*>(*request);

                            ProcessPipelineRequest(pRequest);
                        } break;

                        case GPUResourceRequest::Type::Shader:
                        {
                            auto sRequest = static_cast<ShaderRequest*>(*request);

                            ProcessShaderRequest(sRequest);
                        } break;

                        case GPUResourceRequest::Type::RenderPass:
                        {
                            auto sRequest = static_cast<RenderPassRequest*>(*request);

                            ProcessRenderPassRequest(sRequest);
                        } break;

                        case GPUResourceRequest::Type::RenderTarget:
                        {
                            auto sRequest = static_cast<RenderTargetRequest*>(*request);

                            ProcessRenderTargetRequest(sRequest);
                        } break;

                        case GPUResourceRequest::Type::Mesh:
                        {
                            auto sRequest = static_cast<MeshRequest*>(*request);

                            ProcessMeshRequest(sRequest);
                        } break;

                    }

                    m_processed = true;
                    m_cv.notify_one();
                    
                }

                ReleaseCOM(_allocator);
            }

            void D3D12GPUResourceThread::VCreateTextureBase(Resource::TextureHandle handle, void ** base)
            {
                D3D12Texture** _base = reinterpret_cast<D3D12Texture**>(base);
                if (!*_base)
                {
                    *_base = new D3D12Texture;
                    if (!(*_base)->Initialize(handle, m_device))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Texture Resource.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreateMaterialBase(Resource::MaterialHandle handle, void ** base)
            {
                D3D12Material** _base = reinterpret_cast<D3D12Material**>(base);
                if (!*_base)
                {
                    *_base = new D3D12Material;
                    if (!(*_base)->Initialize(handle, m_device))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Material Resource.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreateRootLayoutBase(Resource::RootLayoutHandle handle, void ** base)
            {
                D3D12RootLayout** _base = reinterpret_cast<D3D12RootLayout**>(base);
                if (!*_base)
                {
                    *_base = new D3D12RootLayout;
                    if (!(*_base)->Initialize(handle, m_device->GetDevice()))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU RootLayout Resource.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreatePipelineBase(Resource::PipelineHandle handle, void ** base)
            {
                D3D12Pipeline** _base = reinterpret_cast<D3D12Pipeline**>(base);
                if (!*_base)
                {
                    *_base = new D3D12Pipeline;
                    if (!(*_base)->Initialize(handle, m_device->GetDevice()))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Pipeline Resource.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreateShaderBase(Resource::ShaderHandle handle, void ** base)
            {
                D3D12Shader** _base = reinterpret_cast<D3D12Shader**>(base);
                if (!*_base)
                {
                    *_base = new D3D12Shader;
                    if (!(*_base)->Initialize(handle))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Shader.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreateRenderPassBase(Resource::RenderPassHandle handle, void** base) 
            {
                D3D12RenderPass** _base = reinterpret_cast<D3D12RenderPass**>(base);
                if (!*_base)
                {
                    *_base = new D3D12RenderPass;
                    if (!(*_base)->Initialize(handle))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Render Pass.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreateRenderTargetBase(Resource::RenderTargetHandle handle, void** base) 
            {
                D3D12RenderTarget** _base = reinterpret_cast<D3D12RenderTarget**>(base);
                if (!*_base)
                {
                    *_base = new D3D12RenderTarget;
                    if (!(*_base)->Initialize(handle))
                    {
                        HT_DEBUG_PRINTF("Failed to initialize GPU Render Target.\n");
                    }
                }
            }

            void D3D12GPUResourceThread::VCreateMeshBase(Resource::ModelHandle handle, void** base) 
            {
                //D3D12Mesh** _base = reinterpret_cast<D3D12Mesh**>(base);
                //if (!*_base)
                //{
                //    *_base = new D3D12Mesh;
                //    if (!(*_base)->Initialize(handle))
                //    {
                //        HT_DEBUG_PRINTF("Failed to initialize GPU Mesh.\n");
                //    }
                //}
            }

        }
    }
}