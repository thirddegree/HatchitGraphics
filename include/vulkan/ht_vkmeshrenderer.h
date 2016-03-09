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

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class HT_API VKMeshRenderer : public IMeshRenderer
            {
            public:
                VKMeshRenderer();
                ~VKMeshRenderer();

                ///Allocates a mesh into Vulkan memory
                void VBuffer()                                  override;

                /* Set which render pass will be bound to this command buffer
                * \param renderPass A pointer to the render pass that this will be a part of
                */
                void VSetRenderPass(IRenderPass* renderPass)    override;

                ///Submit's a command buffer to the VKRenderer
                void VRender()                                  override;

                ///Cleans up a mesh and material from Vulkan memory
                void VFree()                                    override;

            protected:
                VKRenderPass* m_markedRenderPass;
                VKMaterial*  m_material;
                VkCommandBuffer m_commandBuffer;
            };
        }
    }
}