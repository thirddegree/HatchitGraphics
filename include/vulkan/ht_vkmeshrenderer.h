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
* \class VKMeshRenderer
* \ingroup HatchitGraphics
*
* \brief An implementation of the IMeshRenderer interface
*
* Taking in a Mesh and a Material this renderer creates and submits a command buffer
* to the VKRenderer to draw the mesh with the shaders described in the Material's pipeline
*/

#pragma once

#include <ht_meshrenderer.h>
#include <ht_vulkan.h>
#include <ht_vkrenderpass.h>
#include <ht_vkmaterial.h>
#include <ht_vkpipeline.h>
#include <ht_vkrenderer.h>
#include <ht_vkmesh.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKMeshRenderer : public IMeshRenderer
            {
            public:
                VKMeshRenderer();
                ~VKMeshRenderer();

                /* Set which mesh will be rendered
                * \param mesh A pointer to the mesh you want to render
                */
                void VSetMesh(IMesh* mesh)                      override;

                /* Set which material you want to render with
                * \param material the material you want to render with
                * The material should also store the appropriate pipeline
                */
                void VSetMaterial(IMaterial* material)          override;

                /* Set which render pass will be bound to this command buffer
                * \param renderPass A pointer to the render pass that this will be a part of
                */
                void VSetRenderPass(IRenderPass* renderPass)    override;

                ///Submit's a command buffer to the VKRenderer
                void VRender()                                  override;

            protected:
                VKRenderPass*   m_renderPass;
                VKPipeline*     m_pipeline;
                VKMaterial*     m_material;
                VKMesh*         m_mesh;

            };
        }
    }
}