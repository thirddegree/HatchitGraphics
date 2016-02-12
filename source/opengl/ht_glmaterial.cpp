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

#include <ht_glmaterial.h>
#include <ht_debug.h>

namespace Hatchit {

	namespace Graphics {

		GLMaterial::GLMaterial()
		{

		}

		GLMaterial::~GLMaterial()
		{

		}

		void GLMaterial::VOnLoaded() 
		{
			//Take all shaders and compile them into a shader program
			shaderProgram = glCreateProgram();

			for (unsigned int i = 0; i < 6; i++)
			{
				GLShader* glShader = (GLShader*)shaders[i];

				if(glShader != nullptr)
					glAttachShader(shaderProgram, glShader->shader);
			}

			glLinkProgram(shaderProgram);

#ifdef _DEBUG
			printProgramLog();
#endif
		}

		void GLMaterial::printProgramLog() 
		{
			GLint logLength = 0;
			GLsizei charsWritten = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

			if (logLength > 0)
			{
				logLength++; //to account for the null terminator
				char* log = new char(logLength);

				glGetProgramInfoLog(shaderProgram, logLength, &charsWritten, log);
				log[logLength] = '\0';

				Core::DebugPrintF(log);

				delete[] log;
			}
		}
	}
}