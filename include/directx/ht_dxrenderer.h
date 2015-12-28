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

#include <ht_platform.h>
#include <ht_renderer.h>
#include <ht_directx.h>
#include <ht_types.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API DXRenderer : public IRenderer
        {
        public:
            DXRenderer();

            ~DXRenderer();

            bool VInitialize(const RendererParams& params)          override;

            void VDeInitialize()                                    override;

            void VResizeBuffers(uint32_t width, uint32_t height)    override;

            void VSetClearColor(const Color& color)                 override;

            void VClearBuffer(ClearArgs args)                       override;

            void VPresent()                                         override;

        private:
            bool CreateBuffers(uint32_t width, uint32_t height);

        private:
            ID3D11Device*           m_device;
            ID3D11DeviceContext*    m_context;
            ID3D11RenderTargetView* m_renderTargetView;
            ID3D11DepthStencilView* m_depthStencilView;
            IDXGISwapChain*         m_swapChain;
            DXGI_SWAP_CHAIN_DESC    m_swapChainDesc;
            D3D_FEATURE_LEVEL       m_featureLevel;
            Color                   m_clearColor;
            HWND                    m_hwnd;
        };

    }

}
