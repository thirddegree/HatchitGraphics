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
* \class IMeshRenderer
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will render a mesh to the screen
*
* This class will be extended by a class that will implement its
* methods to buffer and draw a mesh for a graphics language
*/

#pragma once

#include <ht_renderpass.h>
#include <ht_objectrenderer.h>
#include <ht_material.h>
#include <ht_mesh.h>
#include <ht_model.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API MeshRenderer : public IObjectRenderer
        {
        public:
            virtual ~MeshRenderer();

            /* Set which material you want to render with
            * \param material the material you want to render with
            * The material should also store the appropriate pipeline
            */
            virtual void SetMaterial(MaterialHandle material);

            /* Set which mesh will be rendered
            * \param mesh A pointer to the mesh you want to render
            */
            virtual void SetMesh(MeshHandle mesh);

            /*Sets the instance data to be used with this particular mesh renderer
            */
            virtual void SetInstanceData(std::vector<Resource::ShaderVariable*> data);

            //Override to submit a render request with a graphics language
            virtual void Render();


        protected:
            RenderPassHandle        m_renderPass;
            PipelineHandle          m_pipeline;
            MaterialHandle          m_material;
            MeshHandle              m_mesh;
            std::vector<Resource::ShaderVariable*> m_instanceData;
        };
    }
}
