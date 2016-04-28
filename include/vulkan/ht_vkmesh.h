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
* \class IMesh
* \ingroup HatchitGraphics
*
* \brief An interface to a mesh that exists on the GPU
*
* You must pass this interface a Resource::Mesh which is a collection
* of data that you want buffered onto the graphics card
*/

#pragma once

#include <ht_mesh.h>
#include <ht_vulkan.h>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            class VKRenderer;
            class HT_API VKMesh : public Core::RefCounted<VKMesh>, public IMesh
            {
            public:
                VKMesh(Core::Guid ID);
                ~VKMesh();

                bool Initialize(Hatchit::Resource::Mesh* mesh, VKRenderer* renderer);

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