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
* \class GSVertShader
* \ingroup HatchitGraphics
*
* \brief A class that will compile and reflect a vertex shader with OpenGL
*
* This should compile and reflect a GLSL vertex shader with OpenGL from a blob of character data
*/

#pragma once

#include <ht_glshader.h>
#include <ht_shader.h>

namespace Hatchit {

	namespace Graphics {

        namespace OpenGL {

            class HT_API GLVertShader : public GLShader
            {
            public:
                GLVertShader();
                virtual ~GLVertShader();

                void VCompile() override;
            };
        }
	
	}
}
