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
#include <ht_gmesh.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API IMeshRenderer : public IObjectRenderer
        {
        public:
            virtual ~IMeshRenderer() {};
            
            /* Set which mesh will be rendered
            * \param mesh A pointer to the mesh you want to render
            */
            virtual void VSetMesh(IMesh* mesh) = 0;

            /* Set which material you want to render with
            * \param material the material you want to render with
            * The material should also store the appropriate pipeline
            */
            virtual void VSetMaterial(IMaterial* material) = 0;

            /* Set which render pass this will be a part of
            * \param renderPass A pointer to the render pass that this will be a part of
            */
            virtual void VSetRenderPass(IRenderPass* renderPass) = 0;

            ///Override to submit a render request with a graphics language
            virtual void VRender() = 0;

            //All resources should be reference counted and freed appropriately
            //virtual void VFree() = 0;
        };
    }
}
