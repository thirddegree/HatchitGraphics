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


#pragma once

#include <ht_platform.h>
#include <ht_pipeline.h>
#include <ht_directx.h>

namespace Hatchit {

    namespace Graphics {

        namespace DX
        {
            class HT_API D3D12Pipeline : public IPipeline
            {
            public:
                D3D12Pipeline();

                // Inherited via IPipeline
                virtual void VSetRasterState(const RasterizerState & rasterState) override;
                virtual void VSetMultisampleState(const MultisampleState & multiState) override;
                virtual void VLoadShader(ShaderSlot shaderSlot, IShader * shader) override;
                virtual bool VPrepare() override;

            private:
                D3D12_GRAPHICS_PIPELINE_STATE_DESC m_description;
                D3D12_RASTERIZER_DESC              m_rasterDesc;
            };
        }
    }
}