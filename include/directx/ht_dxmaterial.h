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
#include <ht_dxvertshader.h>
#include <ht_dxfragshader.h>
#include <ht_dxgeoshader.h>
#include <ht_dxtessshader.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API DXMaterial : public IMaterial
		{
		public:
			DXMaterial();
			virtual ~DXMaterial();
		};
	}
}
