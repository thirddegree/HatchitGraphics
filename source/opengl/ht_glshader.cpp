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

#include <ht_glshader.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace OpenGL {

            GLShader::GLShader()
            {

            }

            GLShader::~GLShader()
            {

            }

            void GLShader::VOnLoaded()
            {
                VCompile();
            }

            bool GLShader::VInitFromFile(Core::File* file)
            {
                size_t size = file->SizeBytes();

                BYTE* blob = new BYTE[size];
                size_t length = file->Read(blob, size - 1);
                blob[length] = '\0';

                m_data = (void*)blob;

                return true;
            }

#ifdef _DEBUG
            void GLShader::LoadDirectlyFromFile(std::string path)
            {
                Core::File shaderFile;
                shaderFile.Open(path, Core::FileMode::ReadText);

                size_t size = shaderFile.SizeBytes();

                BYTE* blob = new BYTE[size];
                size_t length = shaderFile.Read(blob, size - 1);
                blob[length] = '\0';

                m_data = (void*)blob;

                shaderFile.Close();

                VCompile();
            }
#endif

            void GLShader::printShaderLog()
            {
                GLint logLength = 0;
                GLsizei charsWritten = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

                if (logLength > 0)
                {
                    std::vector<GLchar> log(logLength);

                    glGetShaderInfoLog(shader, logLength, &charsWritten, &log[0]);

                    Core::DebugPrintF(&log[0]);

                    glDeleteShader(shader);
                }
            }

            void GLShader::compileGL(GLenum shaderType)
            {
                shader = glCreateShader(shaderType);

                GLchar* string = (GLchar*)m_data;
                size_t sourceSize = strlen(string);

                glShaderSource(shader, 1, (const GLchar**)&string, (GLint*)&sourceSize);

                glCompileShader(shader);

#ifdef _DEBUG
                printShaderLog();
#endif
                //Delete the member data as it is no longer needed
                delete[] m_data;
            }

        }	
    }
}
