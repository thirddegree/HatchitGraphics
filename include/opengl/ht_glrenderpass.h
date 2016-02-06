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
* \class GLRenderPass
* \ingroup HatchitGraphics
*
* \brief A class that acts as a way to render a whole scene to a RenderTexture with OpenGL
*
* Used to render a whole scene to a RenderTexture with OpenGL so that it can be used later
* to complete the final frame. 
*/

#pragma once

#include <ht_renderpass.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API GLRenderPass : public IRenderPass
		{
		public:
			GLRenderPass();
			virtual ~GLRenderPass();

			///Render the pass with OpenGL
			virtual void VRender()		override;
		};
	}
}
