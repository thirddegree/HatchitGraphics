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

#include <ht_vkmesh.h>
#include <ht_vkrenderer.h>
#include <ht_debug.h>

namespace Hatchit {
    namespace Graphics {
        namespace Vulkan {
        
            VKMesh::VKMesh(Core::Guid ID) :
                RefCounted<VKMesh>(std::move(ID))
            {}

            VKMesh::~VKMesh() 
            {
                //Get Device
                VkDevice device = m_renderer->GetVKDevice();

                vkDestroyBuffer(device, m_vertexBlock.buffer, nullptr);
                vkFreeMemory(device, m_vertexBlock.memory, nullptr);

                vkDestroyBuffer(device, m_indexBlock.buffer, nullptr);
                vkFreeMemory(device, m_indexBlock.memory, nullptr);
            }

            bool VKMesh::Initialize(Resource::Mesh* mesh, VKRenderer* renderer)
            {
                m_renderer = renderer;
                //Get Device
                VkDevice device = renderer->GetVKDevice();

                //Generate Vertex Buffer
                std::vector<Vertex> vertexBuffer; 

                std::vector<aiVector3D> verticies = mesh->getVertices();
                std::vector<aiVector3D> normals = mesh->getNormals();
                std::vector<aiVector3D> uvs = mesh->getUVs();

                size_t vertCount = verticies.size();
                size_t normalCount = normals.size();
                size_t uvCount = uvs.size();

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

                if (!m_renderer->CreateBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize, vertexBuffer.data(), &m_vertexBlock))
                    return false;

                if (!m_renderer->CreateBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize, indexBuffer.data(), &m_indexBlock))
                    return false;

                return true;

            }

            UniformBlock_vk VKMesh::GetVertexBlock() { return m_vertexBlock; }
            UniformBlock_vk VKMesh::GetIndexBlock() { return m_indexBlock; }

        }
    }
}