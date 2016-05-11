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

#include <ht_mesh.h>
#include <ht_vkmesh.h>
#include <ht_vkdevice.h>
#include <ht_vktools.h>
#include <ht_debug.h>

namespace Hatchit {
    namespace Graphics {
        namespace Vulkan {
        
            VKMesh::VKMesh()
            {}

            VKMesh::~VKMesh() 
            {
                vkDestroyBuffer(m_device, m_vertexBlock.buffer, nullptr);
                vkFreeMemory(m_device, m_vertexBlock.memory, nullptr);

                vkDestroyBuffer(m_device, m_indexBlock.buffer, nullptr);
                vkFreeMemory(m_device, m_indexBlock.memory, nullptr);
            }

            bool VKMesh::Initialize(Resource::Mesh* mesh, const VkDevice& device)
            {
                m_device = device;

                //Generate Vertex Buffer
                std::vector<Vertex> vertexBuffer; 

                std::vector<aiVector3D> verticies = mesh->getVertices();
                std::vector<aiVector3D> normals = mesh->getNormals();
                std::vector<aiVector3D> uvs = mesh->getUVs();

                for (uint32_t i = 0; i < verticies.size(); i++)
                {
                    Vertex vertex;

                    vertex.pos = verticies[i];
                    if (normals.size() > 0)
                        vertex.norm = normals[i];
                    if(uvs.size() > 0)
                        vertex.uv = aiVector2D(uvs[i][0], uvs[i][1]);

                    vertexBuffer.push_back(vertex);
                }

                size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);

                //Generate Index buffer 
                std::vector<uint32_t> indexBuffer;

                std::vector<aiFace> indicies = mesh->getIndices();

                for (uint32_t i = 0; i < indicies.size(); i++)
                {
                    aiFace face = indicies[i];
                    for (uint32_t f = 0; f < face.mNumIndices; f++)
                        indexBuffer.push_back(face.mIndices[f]);
                }

                m_indexCount = static_cast<uint32_t>(indexBuffer.size());
                size_t indexBufferSize = m_indexCount * sizeof(uint32_t);

                if (!VKTools::CreateUniformBuffer(vertexBufferSize, vertexBuffer.data(), &m_vertexBlock))
                    return false;

                if (!VKTools::CreateUniformBuffer(indexBufferSize, indexBuffer.data(), &m_indexBlock))
                    return false;

                return true;
            }

            uint32_t VKMesh::VGetIndexCount() { return m_indexCount; }

            UniformBlock_vk VKMesh::GetVertexBlock() { return m_vertexBlock; }
            UniformBlock_vk VKMesh::GetIndexBlock() { return m_indexBlock; }

        }
    }
}
