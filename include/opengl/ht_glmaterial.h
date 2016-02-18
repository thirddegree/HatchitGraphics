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

        namespace OpenGL {

            class HT_API GLMaterial : public IMaterial
            {
            public:
                GLMaterial();
                virtual ~GLMaterial();

                void VOnLoaded() override;
                void VSetShader(ShaderSlot shaderSlot, IShader* shader) override;

                //TODO: Remove
                bool VInitFromFile(Core::File* file) override;

                virtual bool VSetInt(std::string name, int data)								override;
                virtual bool VSetFloat(std::string name, float data)							override;
                virtual bool VSetFloat2(std::string name, Math::Vector2 data)					override;
                virtual bool VSetFloat3(std::string name, Math::Vector3 data)					override;
                virtual bool VSetFloat4(std::string name, Math::Vector4 data)					override;
                virtual bool VSetMatrix3(std::string name, Math::Matrix3 data)					override;
                virtual bool VSetMatrix4(std::string name, Math::Matrix4 data)					override;

                virtual bool VBindTexture(std::string name, ITexture* texture)					override;
                virtual bool VUnbindTexture(std::string name, ITexture* texture)				override;

                void VBind()	override;
                void VUnbind()  override;
            private:
                GLuint shaderProgram;
                std::map <std::string, ShaderVariable*> variables;
                std::map <std::string, GLuint> variableLocations;

                void printProgramLog();
                void reflectShaderGL();
            };
        }
	}
}
