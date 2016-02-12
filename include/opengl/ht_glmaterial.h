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
* \class GLMaterial
* \ingroup HatchitGraphics
*
* \brief A material to draw objects with; implemented in OpenGL
*
* This is an extension of IMaterial and extends its methods 
* with ones that will utilize OpenGL calls
*/

#pragma once

#include <ht_material.h>
#include <ht_glvertshader.h>
#include <ht_glfragshader.h>
#include <ht_glgeoshader.h>
#include <ht_gltessshader.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API GLMaterial : public IMaterial
		{
		public:
			GLMaterial();
			virtual ~GLMaterial();

			void VOnLoaded() override;
			
			virtual bool VSetData(std::string name, const void* data, size_t size)			override;
			virtual bool VSetInt(std::string name, int data)								override;
			virtual bool VSetFloat(std::string name, float data)							override;
			virtual bool VSetFloat2(std::string name, const float data[2])					override;
			virtual bool VSetFloat2(std::string name, float x, float y)						override;
			virtual bool VSetFloat3(std::string name, const float data[3])					override;
			virtual bool VSetFloat3(std::string name, float x, float y, float z)			override;
			virtual bool VSetFloat4(std::string name, const float data[4])					override;
			virtual bool VSetFloat4(std::string name, float x, float y, float z, float w)	override;
			virtual bool VSetMatrix4x4(std::string name, const float data[16])				override;

			virtual bool VBindTexture(std::string name, ITexture* texture)					override;
			virtual bool VUnbindTexture(std::string name, ITexture* texture)				override;


		private:
			GLuint shaderProgram;
			std::map <std::string, IShaderVariable*> variables;

			void printProgramLog();
			void reflectShaderGL();
		};
	}
}
