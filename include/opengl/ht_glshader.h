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
* \class GLShader
* \ingroup HatchitGraphics
*
* \brief A class that will load a shader with OpenGL
*
* This should load and compile a GLSL shader with OpenGL
*/

#pragma once

#include <ht_shader.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API GLShader : public IShader
		{
		public:
			GLShader();
			virtual ~GLShader();
		};
	}
}
