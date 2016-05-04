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

#include <ht_platform.h>
#include <ht_model.h>
#include <ht_mesh_resource.h>

namespace Hatchit {
    namespace Graphics {

        class MeshBase;

        struct Vertex
        {
            aiVector3D pos;
            aiVector3D norm;
            aiVector2D uv;
        };

        class HT_API Mesh : public Core::RefCounted<Mesh>
        {
        public:
            Mesh(Core::Guid ID);
            virtual ~Mesh() {};

            bool Initialize(const std::string& file);

            uint32_t GetIndexCount();

            MeshBase* const GetBase() const;

        protected:
            MeshBase* m_base;
        };

        using MeshHandle = Core::Handle<Mesh>;
    }
}