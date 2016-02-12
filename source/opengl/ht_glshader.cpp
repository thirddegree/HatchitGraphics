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

		void GLShader::printShaderLog() 
		{
			GLint logLength = 0;
			GLsizei charsWritten = 0;
			glGetShaderiv(shader,GL_INFO_LOG_LENGTH, &logLength);

			if (logLength > 0)
			{
				logLength++; //to account for the null terminator
				char* log = new char(logLength);
				
				glGetShaderInfoLog(shader, logLength, &charsWritten, log);
				log[logLength] = '\0';
				
				Core::DebugPrintF(log);

				delete[] log;
			}
		}

		void GLShader::compileGL(GLenum shaderType)
		{
			shader = glCreateShader(shaderType);

			size_t sourceSize = strlen((char*)m_data);
			glShaderSource(shader, 1, (GLchar**)&m_data, (GLint*)&sourceSize);

			glCompileShader(shader);

#ifdef _DEBUG
			printShaderLog();
#endif
			//Delete the member data as it is no longer needed
			delete[] m_data;
		}
    }
}
