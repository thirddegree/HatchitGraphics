/**
**    Hatchit Engine
**    Copyright(c) 2015 ThirdDegree
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

#include <ht_d3d12pipeline.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            D3D12Pipeline::D3D12Pipeline()
            {

            }

            void D3D12Pipeline::VSetRasterState(const Resource::Pipeline::RasterizerState & rasterState)
            {
                //Handle cullmode
                switch (rasterState.cullMode)
                {
                case Resource::Pipeline::CullMode::BACK:
                    m_rasterDesc.CullMode = D3D12_CULL_MODE_BACK;
                    break;
                case Resource::Pipeline::CullMode::FRONT:
                    m_rasterDesc.CullMode = D3D12_CULL_MODE_FRONT;
                    break;
                case Resource::Pipeline::CullMode::NONE:
                    m_rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
                    break;
                }

                //Handle polygon mode
                switch (rasterState.polygonMode)
                {
                case Resource::Pipeline::PolygonMode::SOLID:
                    m_rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
                    break;
                case Resource::Pipeline::PolygonMode::LINE:
                    m_rasterDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
                    break;
                }

                m_rasterDesc.DepthClipEnable = rasterState.depthClampEnable;
                m_rasterDesc.FrontCounterClockwise = rasterState.frontCounterClockwise;
                m_rasterDesc.DepthBias = 0;
                m_rasterDesc.DepthBiasClamp = 0.0f;
                m_rasterDesc.SlopeScaledDepthBias = 0.0f;
                m_rasterDesc.MultisampleEnable = false;
                m_rasterDesc.AntialiasedLineEnable = false;
                m_rasterDesc.ForcedSampleCount = 0;
                m_rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
            }

            void D3D12Pipeline::VSetMultisampleState(const Resource::Pipeline::MultisampleState & multiState)
            {
                
            }

            void D3D12Pipeline::VLoadShader(Resource::Pipeline::ShaderSlot shaderSlot, Resource::ShaderHandle shader)
            {
                
            }

            bool D3D12Pipeline::VPrepare()
            {
                //Set rasterize state
                m_description.RasterizerState = m_rasterDesc;

                return false;
            }
        }
    }
}