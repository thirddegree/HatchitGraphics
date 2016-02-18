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

#pragma once

#include <ht_platform.h>
#include <ht_shader.h>
#include <ht_gl.h>
#include <ht_math.h>
#include <map>

namespace Hatchit {

    namespace Graphics {

        namespace OpenGL {

            class HT_API GLShader : public IShader
            {
                friend class GLMaterial;
            public:
                GLShader();

                virtual ~GLShader();

                bool VInitFromFile(Core::File* file) override;

                void VOnLoaded() override;

                bool VSetData(std::string name, const void* data, size_t size) override;
                bool VSetInt(std::string name, int data) override;
                bool VSetFloat(std::string name, float data) override;
                bool VSetFloat2(std::string name, Math::Vector2 data) override;
                bool VSetFloat3(std::string name, Math::Vector3 data) override;
                bool VSetFloat4(std::string name, Math::Vector4 data) override;
                bool VSetMatrix3(std::string name, Math::Matrix3 data) override;
                bool VSetMatrix4(std::string name, Math::Matrix4 data) override;

                bool VBindTexture(std::string name, ITexture* texture) override;
                bool VUnbindTexture(std::string name, ITexture* texture) override;

#ifdef _DEBUG
                void LoadDirectlyFromFile(std::string path);
#endif

            protected:
                GLuint shader;
                std::map<std::string, GLuint> m_uniformMap;
                std::map<std::string, GLuint> m_textureMap;

                void printShaderLog();
                void compileGL(GLenum shaderType);
            };
        }
    }
}
