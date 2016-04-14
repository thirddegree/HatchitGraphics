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
* \class IRenderTarget
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will render the whole scene from a perspective with a graphics language
*
* Used to render a whole scene to an IRenderTexture with a graphics language
* so that it can be used later to complete the final frame.
*/

#pragma once

#include <ht_platform.h>
#include <ht_rendertarget.h>
#include <ht_pipeline.h>
#include <ht_mesh.h>
#include <ht_material.h>
#include <ht_math.h>
#include <ht_color.h>
#include <ht_renderpass_resource.h>

namespace Hatchit {

    namespace Graphics {

        struct RenderRequest 
        {
            IPipelineHandle  pipeline;
            IMaterialHandle  material;
            IMeshHandle      mesh;
        };

        struct Renderable 
        {
            IMaterialHandle  material;
            IMeshHandle      mesh;
        };

        class HT_API IRenderPass
        {
        public:
            virtual ~IRenderPass() { };

            //virtual bool VPrepare() = 0;

            //Will this be sent the Objects that it needs to render?
            ///Render the the given objects with the given pipeline to a texture
            virtual void VUpdate() = 0;

            virtual bool VBuildCommandList() =  0;

            virtual void VSetView(Math::Matrix4 view) = 0;
            virtual void VSetProj(Math::Matrix4 proj) = 0;

            virtual void VScheduleRenderRequest(IPipelineHandle pipeline, IMaterialHandle material, IMeshHandle mesh) = 0;
        };

        class HT_API RenderPassBase : public IRenderPass
        {
        public:
            virtual bool VInitFromResource(const Resource::RenderPassHandle& handle);

            virtual void VSetView(Math::Matrix4 view);
            virtual void VSetProj(Math::Matrix4 proj);

            virtual void VScheduleRenderRequest(IPipelineHandle pipeline, IMaterialHandle material, IMeshHandle mesh);

            uint64_t GetLayerFlags();

        protected:
            void BuildRenderRequestHeirarchy();

            //Input
            std::vector<RenderRequest> m_renderRequests;
            std::map<IPipelineHandle, std::vector<Renderable>> m_pipelineList;

            std::vector<IRenderTargetHandle> m_inputRenderTargets;

            //Output
            std::vector<IRenderTargetHandle> m_outputRenderTargets;

            //Internals
            uint64_t m_layerflags = 1;

            uint32_t m_width;
            uint32_t m_height;

            Math::Matrix4 m_view;
            Math::Matrix4 m_proj;

            Resource::RenderPassHandle m_renderPassResourceHandle;
        };

        using IRenderPassHandle = Core::Handle<IRenderPass>;
        using RenderPassBaseHandle = Core::Handle<RenderPassBase>;
    }
}
