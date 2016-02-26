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

                //Prepare mesh data
                float* meshData = nullptr;

                std::vector<aiVector3D> verticies = mesh->getVertices();
                std::vector<aiVector3D> normals = mesh->getNormals();
                std::vector<aiVector2D> uvs = mesh->getUVs();

                size_t vertCount = verticies.size();
                size_t normalCount = normals.size();
                size_t uvCount = uvs.size();

                m_vertexCount = static_cast<uint32_t>(vertCount);

                size_t vertCompCount = vertCount * 3;
                size_t normalCompCount = normalCount * 3;
                size_t uvCompCount = uvCount * 2;

                size_t meshCompCount = vertCompCount  + normalCompCount + uvCompCount;
                size_t meshDataSize = meshCompCount * sizeof(float);

                meshData = new float[meshCompCount];

                uint32_t collectionIndex;
                uint32_t componentIndex = 0;
                uint32_t i;

                //Store mesh data
                for (collectionIndex = 0; collectionIndex < vertCount; collectionIndex++)
                {
                    for (i = 0; i < 3; i++)
                        meshData[componentIndex++] = verticies[collectionIndex][i];
                }

                //Store normal data
                for (collectionIndex = 0; collectionIndex < normalCount; collectionIndex++)
                {
                    for (i = 0; i < 3; i++)
                        meshData[componentIndex++] = normals[collectionIndex][i];
                }

                //Store UV data
                for (collectionIndex = 0; collectionIndex < uvCount; collectionIndex++)
                {
                    for (i = 0; i < 2; i++)
                        meshData[componentIndex++] = uvs[collectionIndex][i];
                }

                VkResult err;
            
                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferCreateInfo.size = meshDataSize;

                err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to create buffer\n");
#endif
                    //Cleanup float data
                    if (meshData != nullptr)
                        delete[] meshData;

                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(device, m_buffer, &memReqs);

                m_memAllocInfo = {};
                m_memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                m_memAllocInfo.pNext = nullptr;
                m_memAllocInfo.allocationSize = memReqs.size;
                m_memAllocInfo.memoryTypeIndex = 0;

                bool okay = VKRenderer::RendererInstance->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &m_memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to get memory type\n");
#endif
                    //Cleanup float data
                    if (meshData != nullptr)
                        delete[] meshData;

                    return false;
                }

                //Allocate and fill memory
                uint8_t* pData = nullptr;

                err = vkAllocateMemory(device, &m_memAllocInfo, nullptr, &m_memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to allocate memory\n");
#endif
                    //Cleanup float data
                    if (meshData != nullptr)
                        delete[] meshData;

                    return false;
                }

                err = vkMapMemory(device, m_memory, 0, m_memAllocInfo.allocationSize, 0, (void**)pData);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to map memory\n");
#endif
                    //Cleanup float data
                    if (meshData != nullptr)
                        delete[] meshData;

                    return false;
                }

                //Actually copy data into location
                memcpy(pData, &meshData, meshDataSize);

                //Unmap memory and then bind to the uniform
                vkUnmapMemory(device, m_memory);

                err = vkBindBufferMemory(device, m_buffer, m_memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to bind memory\n");
#endif
                    //Cleanup float data
                    if (meshData != nullptr)
                        delete[] meshData;

                    return false;
                }

                m_bufferInfo.buffer = m_buffer;
                m_bufferInfo.offset = 0;
                m_bufferInfo.range = meshDataSize;

                //Cleanup float data
                if (meshData != nullptr)
                    delete[] meshData;

                return true;

            }
            void VKMesh::VFree() 
            {
            
            }

        }
    }
}