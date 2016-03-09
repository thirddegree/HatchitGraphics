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
#include <ht_gmesh.h>
#include <ht_material.h>
#include <ht_math.h>
#include <ht_color.h>

namespace Hatchit {

    namespace Graphics {

        struct RenderRequest 
        {
            IPipeline*  pipeline;
            IMaterial*  material;
            IMesh*      mesh;
        };

        struct Renderable 
        {
            IMaterial*  material;
            IMesh*      mesh;
        };

        class HT_API IRenderPass
        {
        public:
            virtual ~IRenderPass() { };

            virtual bool VPrepare() = 0;

            //Will this be sent the Objects that it needs to render?
            ///Render the the given objects with the given pipeline to a texture
            virtual void VUpdate() = 0;

            virtual bool VBuildCommandList() =  0;

            virtual void VSetClearColor(Color clearColor) = 0;

            void SetWidth(uint32_t width);
            void SetHeight(uint32_t height);

            void SetView(Math::Matrix4 view);
            void SetProj(Math::Matrix4 proj);

            void ScheduleRenderRequest(IPipeline* pipeline, IMaterial* material, IMesh* mesh);

            void SetRenderTarget(IRenderTarget* renderTarget);

        protected:
            void BuildRenderRequestHeirarchy();

            //Input
            std::vector<RenderRequest> m_renderRequests;
            std::map<IPipeline*, std::vector<Renderable>> m_pipelineList;

            uint32_t m_width;
            uint32_t m_height;

            Math::Matrix4 m_view;
            Math::Matrix4 m_proj;

            //Output
            IRenderTarget* m_renderTarget;
        };
    }
}
