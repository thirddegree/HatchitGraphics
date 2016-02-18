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

#include <ht_dxrenderer.h>
#include <ht_debug.h>
#include <DirectXMath.h>

namespace Hatchit {

    namespace Graphics {

        DXRenderer::DXRenderer()
        {
            m_device = nullptr;
            m_context = nullptr;
            m_swapChain = nullptr;
            m_depthStencilView = nullptr;
            m_renderTargetView = nullptr;
        }

        DXRenderer::~DXRenderer()
        {

        }

        bool DXRenderer::VInitialize(const RendererParams& params)
        {
            m_params = params;
            m_hwnd = (HWND)params.window;

            HRESULT hr = S_OK;

            /*Initialize DirectX 11.0 Device*/
            
            UINT createDeviceFlags = 0;
#ifdef _DEBUG
            createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
            D3D_DRIVER_TYPE drivers[] = 
            {
                D3D_DRIVER_TYPE_HARDWARE,
                D3D_DRIVER_TYPE_REFERENCE,
                D3D_DRIVER_TYPE_WARP
            };
            size_t numDrivers = ARRAYSIZE(drivers);

            D3D_FEATURE_LEVEL featureLevels[] =
            {
                D3D_FEATURE_LEVEL_11_0,
            };
            uint32_t numLevels = ARRAYSIZE(featureLevels);

            for (size_t i = 0; i < numDrivers; i++)
            {
                /*Attempt device creation*/
                
                hr = D3D11CreateDevice(nullptr, drivers[i], nullptr, createDeviceFlags,
                    featureLevels, numLevels, D3D11_SDK_VERSION, &m_device, &m_featureLevel, &m_context);

                if (SUCCEEDED(hr))
                    break;
            }
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("Failed to initialize DXRenderer. Exiting.\n");
#endif
                return false;
            }

            RECT r;
            GetClientRect(m_hwnd, &r);
            UINT width = r.right - r.left;
            UINT height = r.bottom - r.top;

            IDXGIDevice* dxgiDevice = nullptr;
            hr = m_device->QueryInterface(__uuidof(IDXGIDevice),
                reinterpret_cast<void**>(&dxgiDevice));
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to query device interface. Exiting.\n");
#endif
                return false;
            }
            IDXGIAdapter* dxgiAdapter = nullptr;
            hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
                reinterpret_cast<void**>(&dxgiAdapter));
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to query adapter interface. Exiting.\n");
#endif
                return false;
            }
            IDXGIFactory* dxgiFactory = nullptr;
            hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
                reinterpret_cast<void**>(&dxgiFactory));
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to query factory interface. Exiting.\n");
#endif
                return false;
            }

            ZeroMemory(&m_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
            m_swapChainDesc.BufferDesc.Width = width;
            m_swapChainDesc.BufferDesc.Height = height;
            m_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
            m_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
            m_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            m_swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            m_swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            m_swapChainDesc.SampleDesc.Count = 1;
            m_swapChainDesc.SampleDesc.Quality = 0;
            m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            m_swapChainDesc.BufferCount = 1;
            m_swapChainDesc.OutputWindow = m_hwnd;
            m_swapChainDesc.Windowed = true;
            m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            m_swapChainDesc.Flags = 0;

            hr = dxgiFactory->CreateSwapChain(m_device, &m_swapChainDesc, &m_swapChain);
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to create swap chain. Exiting.\n");
#endif
                DirectX::ReleaseCOM(dxgiDevice);
                DirectX::ReleaseCOM(dxgiAdapter);
                DirectX::ReleaseCOM(dxgiFactory);
                return false;
            }
            DirectX::ReleaseCOM(dxgiDevice);
            DirectX::ReleaseCOM(dxgiAdapter);
            DirectX::ReleaseCOM(dxgiFactory);

            if (!CreateBuffers(width, height))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to create buffers. Exiting.\n");
#endif
                return false;
            }

            D3D11_VIEWPORT vp;
            vp.TopLeftX = 0.0f;
            vp.TopLeftY = 0.0f;
            vp.Width = static_cast<float>(width);
            vp.Height = static_cast<float>(height);
            vp.MinDepth = 0;
            vp.MaxDepth = 1;

            m_context->RSSetViewports(1, &vp);

            return true;
        }

        void DXRenderer::VDeInitialize()
        {
            if (m_context)
                m_context->ClearState();

            DirectX::ReleaseCOM(m_renderTargetView);
            DirectX::ReleaseCOM(m_depthStencilView);
            DirectX::ReleaseCOM(m_swapChain);
            DirectX::ReleaseCOM(m_context);
            DirectX::ReleaseCOM(m_device);
        }

        void DXRenderer::VSetClearColor(const Color& color)
        {
            m_params.clearColor = color;
        }

        void DXRenderer::VClearBuffer(ClearArgs args)
        {
            switch (args)
            {
            case ClearArgs::Color:
                m_context->ClearRenderTargetView(m_renderTargetView,
                    reinterpret_cast<float*>(&m_params.clearColor));
                break;
            case ClearArgs::ColorDepth:
                m_context->ClearRenderTargetView(m_renderTargetView,
                    reinterpret_cast<float*>(&m_params.clearColor));
                m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
                break;
            case ClearArgs::ColorDepthStencil:
                m_context->ClearRenderTargetView(m_renderTargetView,
                    reinterpret_cast<float*>(&m_params.clearColor));
                m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
                break;
            case ClearArgs::Depth:
                m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
                break;
            case ClearArgs::Stencil:
                m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_STENCIL, 1.0, 0);
                break;

            default:
                break;
            }
        }

        void DXRenderer::VPresent()
        {
            m_swapChain->Present(0, 0);
        }

        void DXRenderer::VResizeBuffers(uint32_t width, uint32_t height)
        {
            DirectX::ReleaseCOM(m_renderTargetView);
            DirectX::ReleaseCOM(m_depthStencilView);

            HRESULT hr = S_OK;
            hr = m_swapChain->ResizeBuffers(1, width, height, m_swapChainDesc.BufferDesc.Format, 0);
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to resize swap chain buffers.\n");
#endif
            }

            if (!CreateBuffers(width, height))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to create buffers.\n");
#endif
            }

            D3D11_VIEWPORT vp;
            vp.TopLeftX = 0.0f;
            vp.TopLeftY = 0.0f;
            vp.Width = static_cast<float>(width);
            vp.Height = static_cast<float>(height);
            vp.MinDepth = 0;
            vp.MaxDepth = 1;

            m_context->RSSetViewports(1, &vp);
        }


        bool DXRenderer::CreateBuffers(uint32_t width, uint32_t height)
        {
            HRESULT hr = S_OK;

            /*Create render target view*/
            ID3D11Texture2D* backBuffer = nullptr;
            hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                reinterpret_cast<void**>(&backBuffer));
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to get buffer from swapchain. Exiting.\n");
#endif
                return false;
            }
            
            hr = m_device->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView);
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to create render target view. Exiting.\n");
#endif
                DirectX::ReleaseCOM(backBuffer);
                return false;
            }
            DirectX::ReleaseCOM(backBuffer);

            /*Create depth/stencil view*/
            D3D11_TEXTURE2D_DESC dsTextureDesc;
            dsTextureDesc.Width = width;
            dsTextureDesc.Height = height;
            dsTextureDesc.MipLevels = 1;
            dsTextureDesc.ArraySize = 1;
            dsTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            dsTextureDesc.SampleDesc.Count = 1;
            dsTextureDesc.SampleDesc.Quality = 0;
            dsTextureDesc.Usage = D3D11_USAGE_DEFAULT;
            dsTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            dsTextureDesc.CPUAccessFlags = 0;
            dsTextureDesc.MiscFlags = 0;

            ID3D11Texture2D* depthStencilBuffer = nullptr;
            hr = m_device->CreateTexture2D(&dsTextureDesc, nullptr, &depthStencilBuffer);
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to create depth stencil buffer for view. Exiting.\n");
#endif
                return false;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
            ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
            dsvDesc.Format = dsTextureDesc.Format;
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;
            hr = m_device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &m_depthStencilView);
            if (FAILED(hr))
            {
#ifdef _DEBUG
                Core::DebugPrintF("[DXRenderer]--Failed to create depth stencil view. Exiting.\n");
#endif
                DirectX::ReleaseCOM(depthStencilBuffer);
                return false;
            }
            DirectX::ReleaseCOM(depthStencilBuffer);

            return true;
        }

    }

}
