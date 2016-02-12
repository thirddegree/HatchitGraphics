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

			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
			{
#ifdef _DEBUG
				printProgramLog();
#endif
				//Do not try to reflect against an improperly compiled shader
				return;
			}

			reflectShaderGL();
		}

		bool GLMaterial::VSetData(std::string name, const void* data, size_t size)			{ return true; } //Eeeeh?
		bool GLMaterial::VSetInt(std::string name, int data)								{ variables[name] = new IntVariable(data); return true;}
		bool GLMaterial::VSetFloat(std::string name, float data)							{ variables[name] = new FloatVariable(data); return true;}
		bool GLMaterial::VSetFloat2(std::string name, const float data[2])					{ variables[name] = new Float2Variable(data[0], data[1]); return true;}
		bool GLMaterial::VSetFloat2(std::string name, float x, float y)						{ variables[name] = new Float2Variable(x, y); return true;}
		bool GLMaterial::VSetFloat3(std::string name, const float data[3])					{ variables[name] = new Float3Variable(data[0], data[1], data[2]); return true;}
		bool GLMaterial::VSetFloat3(std::string name, float x, float y, float z)			{ variables[name] = new Float3Variable(x, y, z); return true;}
		bool GLMaterial::VSetFloat4(std::string name, const float data[4])					{ variables[name] = new Float4Variable(data[0], data[1], data[2], data[3]); return true;}
		bool GLMaterial::VSetFloat4(std::string name, float x, float y, float z, float w)	{ variables[name] = new Float4Variable(x, y, z, w); return true;}
		bool GLMaterial::VSetMatrix4x4(std::string name, const float data[16])				{ return true; }

		bool GLMaterial::VBindTexture(std::string name, ITexture* texture)					{ return true; }
		bool GLMaterial::VUnbindTexture(std::string name, ITexture* texture)				{ return true; }

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

		void GLMaterial::reflectShaderGL() 
		{
			
		}
	}
}