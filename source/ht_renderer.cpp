/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
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

#include <ht_renderer.h>
#include <ht_gpuresourcepool.h>
#include <ht_shadervariablechunk.h>

#ifdef DX12_SUPPORT
#include <ht_d3d12device.h>
#include <ht_d3d12swapchain.h>
#endif

#ifdef VK_SUPPORT
#include <ht_vkdevice.h>
#include <ht_vkswapchain.h>
#include <ht_vkqueue.h>
#include <ht_vktools.h>
#include <ht_vkrenderthread.h>
#endif


namespace Hatchit {

    namespace Graphics {

        IDevice*        Renderer::_Device = nullptr;
        GPUQueue*       Renderer::_Queue = nullptr;
        RendererType    Renderer::_Type = UNKNOWN;
        SwapChain*      Renderer::_SwapChain = nullptr;

        void Renderer::RegisterRenderRequest(RenderPassHandle pass, MaterialHandle material, MeshHandle mesh, ShaderVariableChunk* instanceVariables)
        {
            pass->ScheduleRenderRequest(material, mesh, instanceVariables);

            //Check if the pass already exists
            for (size_t i = 0; i < m_renderPassLayers.size(); i++)
            {
                std::vector<RenderPassHandle> layer = m_renderPassLayers[i];
                for (size_t j = 0; j < layer.size(); j++)
                {
                    if (layer[j] == pass)
                        return;
                }
            }

            uint64_t flags = pass->GetLayerFlags();
            for (int j = 0; flags != 0; j++)
            {
                if (flags & 1)
                {
                    m_renderPassLayers[j].push_back(pass);
                }
                flags >>= 1;
            }
        }

        void Renderer::RegisterCamera(Camera camera)
        {
            uint64_t flags = camera.GetLayerFlags();
            for (int j = 0; flags != 0; j++)
            {
                if (flags & 1)
                {
                    m_renderPassCameras[j].push_back(camera);
                }
                flags >>= 1;
            }
        }

        Renderer::Renderer()
        {
            _SwapChain = nullptr;
            m_locked = false;
            m_processed = false;
        }

        Renderer::~Renderer()
        {
            delete _Device;
            delete _Queue;
            delete _SwapChain;
        }

        bool Renderer::Initialize(const RendererParams& params)
        {
            m_params = params;

            /*Initialize the swapchain and device*/
            switch (params.renderer)
            {
#ifdef DX12_SUPPORT
                case RendererType::DIRECTX12:
                {
                    if (!_Device)
                    {
                        _Device = new DX::D3D12Device;
                        if (!_Device->VInitialize())
                            return false;
                        _Type = RendererType::DIRECTX12;
                    }

                    _SwapChain = new DX::D3D12SwapChain((HWND)params.window);

                    /*Initialize GPU Resource Pool*/
                    GPUResourcePool::Initialize(_Device, _SwapChain);

                    if (!_SwapChain->VInitialize(params.viewportWidth, params.viewportHeight))
                        return false;

                } break;
#endif
#ifdef VK_SUPPORT
                case RendererType::VULKAN:
                {

                    if (!_Device)
                    {
                        Vulkan::VKDevice* Device = new Vulkan::VKDevice;
                        Device->SetValidation(params.validate);
                        Vulkan::VKQueue* Queue = new Vulkan::VKQueue(QueueType::GRAPHICS);
                        if (!Device->VInitialize())
                            return false;
                        if (!Queue->Initialize(Device))
                            return false;

                        _Device = static_cast<IDevice*>(Device);
                        _Queue = static_cast<GPUQueue*>(Queue);
                        _Type = RendererType::VULKAN;

                        if (!Vulkan::VKTools::Initialize(Device, Queue))
                            return false;
                    }

                    _SwapChain = new Vulkan::VKSwapChain(params, static_cast<Vulkan::VKDevice*>(_Device), static_cast<Vulkan::VKQueue*>(_Queue));

                    /*Initialize GPU Resource Pool*/
                    GPUResourcePool::Initialize(_Device, _SwapChain);

                    if (!_SwapChain->VInitialize(params.viewportWidth, params.viewportHeight))
                        return false;
                } break;
#endif
                default:
                    return false;
            }

            initThreads();

            return true;
        }

        void Renderer::ResizeBuffers(uint32_t width, uint32_t height)
        {
            if (!_SwapChain)
                return;

            _SwapChain->VResize(width, height);
        }

        void Renderer::Render()
        {
            //Tell the swapchain which render pass to put on screen
            std::vector<RenderPassHandle> lastLayer = m_renderPassLayers[0];
            RenderPassHandle lastRenderPass = lastLayer[lastLayer.size() - 1];
            _SwapChain->VSetInput(lastRenderPass);

            //Step 01: Clear the buffer
            //Not exactly sure how this will work, as the clear command
            //need to be recorded as part of a command list
            _SwapChain->VClear(reinterpret_cast<float*>(&m_params.clearColor));

            //Step 02: Record each renderpass's command list in a pass thread
            //We will need to *SMARTLY* spawn enough threads to handle
            //the command generation or each render layer.
            for (size_t i = 0; i < m_renderPassLayers.size(); i++)
            {
                std::vector<RenderPassHandle> renderPasses = m_renderPassLayers[i];
                std::vector<Camera> cameras = m_renderPassCameras[i];
                for (size_t j = 0; j < renderPasses.size(); j++)
                {
                    RenderPassHandle passHandle = renderPasses[j];
                    Camera camera = cameras[0];

                    passHandle->SetView(camera.GetView());
                    passHandle->SetProj(camera.GetProjection());
                    
                    //Add work for the render threads
                    m_threadQueue.push(passHandle);
                }
            }

            //Tell all threads that they can work
            for (size_t i = 0; i < this->m_threads.size(); i++)
                m_threads[i]->Notify();
            
            //Wait for all threads to finish
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() -> bool 
            {
                m_locked = true;  
                
                for (size_t i = 0; i < this->m_threads.size(); i++)
                {
                    if (!m_threads[i]->Processed())
                        return false;
                }

                return true; 
            });
            m_locked = false;

            //Step 03: Execute the recorded command lists
            //This is a complicated step as we must execute command lists potentially
            //while others are still being generated. The pass threads must signal they have completed
            //to the renderer for it to then collect and execute them
            for (size_t i = 0; i < m_renderPassLayers.size(); i++)
            {
                _SwapChain->VExecute(m_renderPassLayers[i]);
            }

            //Step 04: Present to the screen
            //The previous step and this are quite related.
            //The execution of the previous command lists may also include,
            //the commands for presenting to the buffer.
            _SwapChain->VPresent();

            //Clear out cameras
            for (size_t i = 0; i < m_renderPassCameras.size(); i++)
            {
                m_renderPassCameras[i].clear();
            }
        }

        void Renderer::Present()
        {

        }
     
        IDevice * const Renderer::GetDevice()
        {
            return Renderer::_Device;
        }
        
        SwapChain* const Renderer::GetSwapChain()
        {
            return Renderer::_SwapChain;
        }

        RendererType Renderer::GetType()
        {
            return Renderer::_Type;
        }

        /*
            Protected Methods
        */

        void Renderer::initThreads() 
        {
            uint32_t threadCount = std::thread::hardware_concurrency();
            for (uint32_t i = 0; i < threadCount; i++)
            {
                RenderThread* renderThread;

#ifdef DX12_SUPPORT
#endif

#ifdef VK_SUPPORT
                renderThread = new Vulkan::VKRenderThread(static_cast<Vulkan::VKDevice*>(_Device));
#endif

                renderThread->VStart(&m_cv, &m_threadQueue);

                m_threads.push_back(renderThread);
            }
        }
    }
}