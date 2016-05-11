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

/**
* \class RenderPassBase
* \ingroup HatchitGraphics
*
* \brief An abstraction of a class that will render the whole scene from a perspective with a graphics language
*
*/

#pragma once

#include <ht_platform.h>
#include <ht_math.h>                //Math::Matrix4
#include <ht_renderpass_resource.h> //Resource::RenderPass
#include <ht_shadervariablechunk.h>

#include <ht_material.h>            //MaterialHandle
#include <ht_mesh.h>                //MeshHandle
#include <ht_rendertarget.h>        //RenderTargetHandle
#include <ht_commandpool.h>         //ICommandPool

namespace Hatchit
{
    namespace Graphics
    {
        struct RenderRequest
        {
            PipelineHandle  pipeline;
            MaterialHandle  material;
            MeshHandle      mesh;
        };

        struct Renderable
        {
            MaterialHandle  material;
            MeshHandle      mesh;
        };

        struct RenderableInstances
        {
            Renderable  renderable;
            uint32_t    count;
        };

        class HT_API RenderPassBase
        {
        public:
            virtual ~RenderPassBase() { };

            virtual void VUpdate() = 0;

            void SetView(Math::Matrix4 view);
            void SetProj(Math::Matrix4 proj);

            virtual bool VBuildCommandList(const ICommandPool* commandPool) = 0;

            virtual void ScheduleRenderRequest(MaterialHandle material, MeshHandle mesh, ShaderVariableChunk* instanceVariables);

            virtual uint64_t GetLayerFlags();

        protected:
            void BuildRenderRequestHeirarchy();

            //Input
            std::vector<RenderRequest> m_renderRequests;
            std::map<PipelineHandle, std::vector<RenderableInstances>> m_pipelineList;
            BYTE* m_instanceData;
            size_t m_instanceDataSize;
            size_t m_currentInstanceDataOffset;
            size_t m_instanceChunkSize;

            //Output
            std::vector<RenderTargetHandle> m_outputRenderTargets;

            //Internals
            uint64_t m_layerflags = 1;

            uint32_t m_width;
            uint32_t m_height;

            Math::Matrix4 m_view;
            Math::Matrix4 m_proj;
        };
    }
}