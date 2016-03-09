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
* \class DXMeshRenderer
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will render a mesh to the screen with DirectX
*
* An extension of the IMeshRenderer class that will implement its methods with 
* DirectX calls
*/

#pragma once

#include <ht_meshrenderer.h>
#include <ht_directx.h>

namespace Hatchit {

	namespace Graphics {

        namespace DirectX {

            class HT_API D3D11MeshRenderer : public IMeshRenderer
            {
            public:
                D3D11MeshRenderer();
                virtual ~D3D11MeshRenderer();

                ///Buffer a mesh with DirectX
                virtual void VBuffer()		override;
                ///Render a mesh with a DirectX
                virtual void VRender()		override;
                ///Gree a mesh from DirectX
                virtual void VFree()		override;

            private:

            };

        }
	}
}
