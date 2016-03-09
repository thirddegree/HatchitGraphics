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
* \class GLMeshRenderer
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will render a mesh to the screen with OpenGL
*
* An extension of the IMeshRenderer class that will implement its methods with 
* OpenGL calls
*/

#pragma once

#include <ht_meshrenderer.h>
#include <ht_glmaterial.h>
#include <ht_gl.h>

namespace Hatchit {

	namespace Graphics {

        namespace OpenGL {

            class HT_API GLMeshRenderer : public IMeshRenderer
            {
            public:
                GLMeshRenderer(Resource::MeshPtr mesh, GLMaterial* material);
                virtual ~GLMeshRenderer();

                ///Buffer a mesh with OpenGL
                virtual void VBuffer()		override;
                ///Render a mesh with a OpenGL
                virtual void VRender()		override;
                ///Gree a mesh from OpenGL
                virtual void VFree()		override;

            private:
                GLuint vbo, vao, ibo;
            };
        }
	}
}
