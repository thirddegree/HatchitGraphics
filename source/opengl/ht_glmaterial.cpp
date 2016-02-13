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
			for (int i = 0; i < 6; i++)
				shaders[i] = nullptr;
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

		void GLMaterial::VSetShader(ShaderSlot shaderSlot, IShader* shader)
		{
			shaders[shaderSlot] = shader;
		}

		bool GLMaterial::VSetInt(std::string name, int data)								{ variables[name] = new IntVariable(data); return true;}
		bool GLMaterial::VSetFloat(std::string name, float data)							{ variables[name] = new FloatVariable(data); return true;}
		bool GLMaterial::VSetFloat2(std::string name, Math::Vector2 data)					{ variables[name] = new Float2Variable(data); return true;}
		bool GLMaterial::VSetFloat3(std::string name, Math::Vector3 data)					{ variables[name] = new Float3Variable(data); return true;}
		bool GLMaterial::VSetFloat4(std::string name, Math::Vector4 data)					{ variables[name] = new Float4Variable(data); return true;}
		bool GLMaterial::VSetMatrix3(std::string name, Math::Matrix3 data)					{ variables[name] = new Matrix3Variable(data); return true; }
		bool GLMaterial::VSetMatrix4(std::string name, Math::Matrix4 data)					{ variables[name] = new Matrix4Variable(data); return true; }

		bool GLMaterial::VBindTexture(std::string name, ITexture* texture)					{ return true; }
		bool GLMaterial::VUnbindTexture(std::string name, ITexture* texture)				{ return true; }

		void GLMaterial::VBind() 
		{
			glUseProgram(shaderProgram);

			for (auto iter : variables)
			{
				ShaderVariable* var = iter.second;
				ShaderVariable::Type t = var->GetType();
				void* data = var->GetData();

				std::string name = iter.first;
				GLuint location = variableLocations[name];

				switch(t)
				{
				case ShaderVariable::FLOAT:
					glUniform1f(location, *(float*)data);
				}
			}
		}

		void GLMaterial::VUnbind() 
		{
			glUseProgram(0);
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

		void GLMaterial::reflectShaderGL() 
		{
			glUseProgram(shaderProgram);

			//Get total count of uniforms
			GLint totalUniforms;
			glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &totalUniforms);

			//Loop over all uniform indexes and get the uniforms
			for (int i = 0; i < totalUniforms; i++)
			{
				GLsizei nameLength, maxLength;
				GLint size;
				GLenum type = GL_ZERO;

				maxLength = 100;
				GLchar* name = new GLchar[maxLength];

				glGetActiveUniform(shaderProgram, i, maxLength - 1, &nameLength, &size, &type, name);
				name[nameLength] = '\0'; 

				//Handle Texture Types
				if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_SHADOW)
				{

				}
				else 
				{
					GLint location = glGetUniformLocation(shaderProgram, name);
					variableLocations[std::string(name)] = location;
				}

				if(name != nullptr)
					delete[] name;
			}
		}
	}
}