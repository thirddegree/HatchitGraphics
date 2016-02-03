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
* \class DXFragShader
* \ingroup HatchitGraphics
*
* \brief A class that will compile and reflect a fragment shader with DirectX
*
* This should compile and reflect an HLSL fragment shader with DirectX from a blob of character data
*/

#pragma once

#include <ht_shader.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API DXFragShader : public IShader
		{
		public:
			DXFragShader();
			virtual ~DXFragShader();
		};
	}
}
