/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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

#pragma once

#include <ht_platform.h>
#include <ht_directx.h>
#include <ht_mesh_base.h>
#include <ht_model.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class HT_API D3D12Mesh : public MeshBase
            {
            public:
                D3D12Mesh();

                ~D3D12Mesh();

                bool Initialize(Resource::ModelHandle handle);


                // Inherited via MeshBase
                virtual uint32_t VGetIndexCount() override;

            private:

            };
        }
    }
}