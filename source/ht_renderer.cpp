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

#ifdef DX12_SUPPORT
#include <ht_d3d12device.h>
#include <ht_d3d12swapchain.h>
#endif

#ifdef VK_SUPPORT
#include <ht_vkdevice.h>
#include <ht_vkswapchain.h>
#include <ht_vkqueue.h>
#include <ht_vktools.h>
#endif


namespace Hatchit {

    namespace Graphics {

        IDevice*        Renderer::_Device = nullptr;
        GPUQueue*       Renderer::_Queue = nullptr;
        RendererType    Renderer::_Type = UNKNOWN;

        void Renderer::RegisterRenderPass(RenderPassHandle pass)
        {
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
            m_swapChain = nullptr;
        }

        Renderer::~Renderer()
        {
            delete _Device;
            delete m_swapChain;
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

                    m_swapChain = new DX::D3D12SwapChain((HWND)params.window);

                    /*Initialize GPU Resource Pool*/
                    GPUResourcePool::Initialize(_Device, m_swapChain);

                    if (!m_swapChain->VInitialize(params.viewportWidth, params.viewportHeight))
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

                    m_swapChain = new Vulkan::VKSwapChain(params, static_cast<Vulkan::VKDevice*>(_Device), static_cast<Vulkan::VKQueue*>(_Queue));

                    /*Initialize GPU Resource Pool*/
                    GPUResourcePool::Initialize(_Device, m_swapChain);

                    if (!m_swapChain->VInitialize(params.viewportWidth, params.viewportHeight))
                        return false;
                } break;
#endif
                default:
                    return false;
            }

            return true;
        }

        void Renderer::ResizeBuffers(uint32_t width, uint32_t height)
        {
            if (!m_swapChain)
                return;

            m_swapChain->VResize(width, height);
        }

        void Renderer::Render()
        {
            //Step 01: Clear the buffer
            //Not exactly sure how this will work, as the clear command
            //need to be recorded as part of a command list
            m_swapChain->VClear(reinterpret_cast<float*>(&m_params.clearColor));

            //Step 02: Record each renderpass's command list in a pass thread
            //We will need to *SMARTLY* spawn enough threads to handle
            //the command generation or each render layer.
            for (size_t i = 0; i < m_renderPassLayers.size(); i++)
            {
                std::vector<RenderPassHandle> renderPasses = m_renderPassLayers[i];
                for (size_t j = 0; j < renderPasses.size(); j++)
                {
                    RenderPassHandle passHandle = renderPasses[j];
                    assert(passHandle->BuildCommandList()); //This needs to succeed
                }
            }

            //Step 03: Execute the recorded command lists
            //This is a complicated step as we must execute command lists potentially
            //while others are still being generated. The pass threads must signal they have completed
            //to the renderer for it to then collect and execute them

            //Step 04: Present to the screen
            //The previous step and this are quite related.
            //The execution of the previous command lists may also include,
            //the commands for presenting to the buffer.
            m_swapChain->VPresent();
        }

        void Renderer::Present()
        {

        }
     
        IDevice * const Renderer::GetDevice()
        {
            return Renderer::_Device;
        }
        
        RendererType Renderer::GetType()
        {
            return Renderer::_Type;
        }
    }
}