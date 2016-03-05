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

#include <ht_d3d12renderer.h>
#include <ht_debug.h>
#include <wrl.h>
#include <ht_file.h>
#include <ht_os.h>
#include <ht_math.h>

#include <ht_model.h>
#include <ctime>

namespace Hatchit {

    namespace Graphics {

        namespace DirectX {

            D3D12Renderer::D3D12Renderer()
            {
                m_pipelineState = nullptr;
                m_rootSignature = nullptr;
                m_vertexBuffer = nullptr;
                m_vBuffer = nullptr;
                m_iBuffer = nullptr;
               
            }

            D3D12Renderer::~D3D12Renderer()
            {
                delete m_resources;
            }

            bool D3D12Renderer::VInitialize(const RendererParams& params)
            {
                m_clearColor = params.clearColor;

                m_resources = new D3D12DeviceResources;
                if (!m_resources->Initialize((HWND)params.window, params.viewportWidth, params.viewportHeight))
                    return false;
                
                return true;
            }

            void D3D12Renderer::VDeInitialize()
            {
                m_resources->WaitForGPU();
            }

            void D3D12Renderer::VSetClearColor(const Color& color)
            {
                m_clearColor = color;
            }

            void D3D12Renderer::VClearBuffer(ClearArgs args)
            {
                
            }

            void D3D12Renderer::VPresent()
            {

                m_resources->Present();
            }

            void D3D12Renderer::VResizeBuffers(uint32_t width, uint32_t height)
            {

            }

            void D3D12Renderer::VRender()
            {

            }

        }

    }

}