#include <ht_vkmesh.h>
#include <ht_vkrenderer.h>
#include <ht_debug.h>

namespace Hatchit {
    namespace Graphics {
        namespace Vulkan {
        
            VKMesh::VKMesh() {}
            VKMesh::~VKMesh() 
            {
                VFree();
            }

            bool VKMesh::VBuffer(Resource::Mesh* mesh) 
            {
                //Get Device
                VkDevice device = VKRenderer::RendererInstance->GetVKDevice();

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

                if (!createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize, vertexBuffer.data(), &m_vertexBuffer, &m_vertexMemory))
                    return false;

                if (!createBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize, indexBuffer.data(), &m_indexBuffer, &m_indexMemory))
                    return false;

                return true;

            }
            void VKMesh::VFree() 
            {
                //Get Device
                VkDevice device = VKRenderer::RendererInstance->GetVKDevice();

                vkDestroyBuffer(device, m_vertexBuffer, nullptr);
                vkFreeMemory(device, m_vertexMemory, nullptr);

                vkDestroyBuffer(device, m_indexBuffer, nullptr);
                vkFreeMemory(device, m_indexMemory, nullptr);
            }
            
            bool VKMesh::createBuffer(VkDevice device, VkBufferUsageFlagBits usage, size_t dataSize, void* data, VkBuffer* buffer, VkDeviceMemory* memory)
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = usage;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to create buffer\n");
#endif
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(device, m_vertexBuffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = VKRenderer::RendererInstance->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to get memory type\n");
#endif
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to allocate memory\n");
#endif
                    return false;
                }

                err = vkMapMemory(device, *memory, 0, dataSize, 0, &pData);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to map memory\n");
#endif
                    return false;
                }

                //Actually copy data into location
                memcpy(pData, data, dataSize);

                //Unmap memory and then bind to the uniform
                vkUnmapMemory(device, *memory);

                err = vkBindBufferMemory(device, *buffer, *memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to bind memory\n");
#endif
                    return false;
                }

                return true;
            }

        }
    }
}