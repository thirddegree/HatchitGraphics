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

#include <ht_d3d12mesh.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            D3D12Mesh::D3D12Mesh()
            {

            }

            D3D12Mesh::~D3D12Mesh()
            {

            }

            bool D3D12Mesh::Initialize(Resource::ModelHandle handle)
            {
                return true;
            }

            uint32_t D3D12Mesh::VGetIndexCount()
            {
                return uint32_t();
            }


        }
    }
}