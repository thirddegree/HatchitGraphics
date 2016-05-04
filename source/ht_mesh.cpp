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
* \class MeshBase
* \ingroup HatchitGraphics
*
* \brief An abstraction of a mesh that exists on the GPU
*
* You must pass this interface a Resource::Mesh which is a collection
* of data that you want buffered onto the graphics card
*/

#include <ht_mesh.h>
#include <ht_mesh_base.h>
#include <cstdint>
#include <ht_gpuresourcepool.h>

namespace Hatchit 
{
    namespace Graphics 
    {

        Mesh::Mesh(Core::Guid ID) 
        {
        }

        bool Mesh::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the pipeline base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateMesh(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request pipeline immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestMesh(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        uint32_t Mesh::GetIndexCount() { return m_base->VGetIndexCount(); }

        MeshBase* const Mesh::GetBase() const
        {
            return m_base;
        }

    }
}