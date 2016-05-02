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
* \class VKMesh
* \ingroup HatchitGraphics
*
* \brief A Mesh existing on the GPU via Vulkan
*/

#pragma once

#include <ht_mesh_base.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class VKRenderer;
            class HT_API VKMesh : public MeshBase
            {
            public:
                VKMesh();
                ~VKMesh();

                bool Initialize(Hatchit::Resource::Mesh* mesh, VKRenderer* renderer);

                uint32_t VGetIndexCount() override;

                UniformBlock_vk GetVertexBlock();
                UniformBlock_vk GetIndexBlock();

            private:
                VKRenderer* m_renderer;
                UniformBlock_vk m_vertexBlock;
                UniformBlock_vk m_indexBlock;
            };

            using VKMeshHandle = Core::Handle<VKMesh>;

        }

    }

}