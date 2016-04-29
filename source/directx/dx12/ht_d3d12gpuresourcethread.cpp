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
#include <ht_debug.h>

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

            void D3D12GPUResourceThread::VLoad(GPUResourceRequest request)
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
                hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_allocator));
                if (FAILED(hr))
                {
                    HT_ERROR_PRINTF("Failed to create command allocator in thread.\n");
                    VKill();
                }

                while (m_alive)
                {
                    /*Load any resource requests*/
                    
                    if (m_requests.empty())
                        continue;
                    
                    auto request = m_requests.pop();
                    switch (request->type)
                    {
                        case GPUResourceRequest::Type::Texture:
                        {
                            HT_DEBUG_PRINTF("GPU Resource Request\n");
                            HT_DEBUG_PRINTF("\tFile: %s\n", request->file);
                            HT_DEBUG_PRINTF("\tType: Texture\n");

                            try
                            {
                                Resource::TextureHandle handle = Resource::Texture::GetHandleFromFileName(request->file);
                                if (!handle.IsValid())
                                {
                                    HT_DEBUG_PRINTF("\tFailed to load resource handle.\n");
                                }
                            }
                            catch (std::exception& e)
                            {
                                HT_ERROR_PRINTF("%s", e.what());
                            }
                           
                        } break;
                    }
                    
                }

                ReleaseCOM(_allocator);
            }
        }
    }
}