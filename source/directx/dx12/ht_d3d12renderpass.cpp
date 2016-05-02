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

#include <ht_d3d12renderpass.h>
#include <ht_renderpass_resource.h>
#include <ht_debug.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12RenderPass::D3D12RenderPass()
            {

            }

            bool D3D12RenderPass::Initialize(const std::string & fileName)
            {
                using namespace Resource;

                Resource::RenderPassHandle handle = Resource::RenderPass::GetHandleFromFileName(fileName);
                if (!handle.IsValid())
                {
                    HT_DEBUG_PRINTF("Error: Tried to load D3D12RenderPass but the resource handle was invalid!\n");
                    return false;
                }

                auto inputPaths = handle->GetInputTargets();
                auto outputPaths = handle->GetOutputPaths();

                for (auto path : inputPaths)
                {

                }

                return false;
            }

            void D3D12RenderPass::VUpdate()
            {

            }

            bool D3D12RenderPass::VBuildCommandList()
            {
                return false;
            }


        }
    }
}