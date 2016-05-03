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

namespace Hatchit {
    namespace Graphics {

        uint32_t Mesh::GetIndexCount() { return m_base->VGetIndexCount(); }

        MeshBase* const Mesh::GetBase() const
        {
            return m_base;
        }

    }
}