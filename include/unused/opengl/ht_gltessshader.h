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
* \class GLTessShader
* \ingroup HatchitGraphics
*
* \brief A class that will compile and reflect a tesselation shader with OpenGL
*
* This should compile and reflect a GLSL tesselation shader with OpenGL from a blob of character data
*/

#pragma once

#include <ht_glshader.h>

namespace Hatchit {

	namespace Graphics {

        namespace OpenGL {

            class HT_API GLTessShader : public GLShader
            {
            public:
                GLTessShader();
                virtual ~GLTessShader();

                void VCompile() override;
            };
        }
	}
}
