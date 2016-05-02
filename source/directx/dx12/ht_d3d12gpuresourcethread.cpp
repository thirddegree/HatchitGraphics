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
#include <ht_debug.h>
#include <ht_gpuresourcerequest.h>
#include <ht_texture_resource.h>

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
                VKill();
            }

            void D3D12GPUResourceThread::VStart()
            {
                m_alive = true;

                m_thread = std::thread(&D3D12GPUResourceThread::thread_main, this);
            }

            bool D3D12GPUResourceThread::VLocked() const
            {
                return m_locked;
            }

            void D3D12GPUResourceThread::VLoad(GPUResourceRequest* request)
            {
                if (!m_alive)
                    VStart();

                m_processed = false;

                m_requests.push(request);
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]() -> bool { m_locked = true;  return this->m_processed; });
                m_locked = false;
            }

            void D3D12GPUResourceThread::VLoadAsync(GPUResourceRequest* request)
            {
                if (!m_alive)
                    VStart();

                m_requests.push(request);
            }

            void D3D12GPUResourceThread::VKill()
            {
                m_alive = false;
                if (m_thread.joinable())
                    m_thread.join();
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
                    VKill();
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

                    }

                    m_processed = true;
                    m_cv.notify_one();
                    
                }

                ReleaseCOM(_allocator);
            }

             
            void D3D12GPUResourceThread::ProcessTextureRequest(TextureRequest * request)
            {
                Resource::TextureHandle handle = Resource::Texture::GetHandle(request->file, request->file);
                if (!m_locked)
                {
                    HT_DEBUG_PRINTF("Async load.\n");
                    
                }
                else
                {
                    HT_DEBUG_PRINTF("Non-Async load.\n");

                    D3D12Texture** base = reinterpret_cast<D3D12Texture**>(request->data);
                    if (!*base)
                    {
                        *base = new D3D12Texture;
                        if (!(*base)->Initialize(handle, m_device))
                        {
                            HT_DEBUG_PRINTF("Failed to initialize GPU Texture Resource.\n");
                        }
                    }
                }
            }

            void D3D12GPUResourceThread::ProcessMaterialRequest(MaterialRequest * request)
            {
                Resource::MaterialHandle handle = Resource::Material::GetHandle(request->file, request->file);
                if (!m_locked)
                {
                    HT_DEBUG_PRINTF("Async load.\n");

                }
                else
                {
                    HT_DEBUG_PRINTF("Non-Async load.\n");

                    D3D12Material** base = reinterpret_cast<D3D12Material**>(request->data);
                    if (!*base)
                    {
                        *base = new D3D12Material;
                        if (!(*base)->Initialize(handle, m_device))
                        {
                            HT_DEBUG_PRINTF("Failed to initialize GPU Material Resource.\n");
                        }
                    }
                }
            }

                
        }
    }
}