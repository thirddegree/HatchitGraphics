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
* \class DXRenderTarget
* \ingroup HatchitGraphics
*
* \brief A class that utilizes RenderTargets to allow the user to write textures with DirectX renders
*
* An implementation of the IRenderTarget class that utilizes DirectX and RenderTargets
* to provide an interface for a texture that can be rendered to
*/

#pragma once

#include <ht_rendertarget.h>
#include <ht_directx.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API DXRenderTarget : public IRenderTarget
		{
		public:
			DXRenderTarget();
			virtual ~DXRenderTarget();

			///Bind the texture for reading
			virtual void VReadBind()		override;
			///Bind the RenderTarget to be rendered to
			virtual void VWriteBind()		override;
			///Free the texture and RenderTarget from memory
			virtual void VFree()			override;

		private:
			
		};
	}
}
