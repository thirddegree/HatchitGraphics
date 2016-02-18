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
* \class DXMaterial
* \ingroup HatchitGraphics
*
* \brief A material to draw objects with; implemented in DirectX
*
* This is an extension of IMaterial and extends its methods 
* with ones that will utilize DirectX calls
*/

#pragma once

#include <ht_material.h>
#include <ht_d3d11vertexshader.h>
#include <ht_d3d11pixelshader.h>

namespace Hatchit {

	namespace Graphics {

        namespace DirectX
        {
            class HT_API D3D11Material : public IMaterial
            {
            public:
                D3D11Material();

                virtual ~D3D11Material();
            };
        }
	}
}
