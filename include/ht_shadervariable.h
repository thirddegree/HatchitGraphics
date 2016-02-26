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
#include <ht_string.h>
#include <ht_math.h>

namespace Hatchit {

	namespace Graphics {

		class IShader;

		class HT_API ShaderVariable 
		{
		public:
			virtual ~ShaderVariable() { };
			enum Type
			{
				INT,
				FLOAT,
				DOUBLE,
				FLOAT2,
				FLOAT3,
				FLOAT4,
				MAT3,
				MAT4
			};

			inline Type GetType() { return m_type; }
			inline void* GetData() { return m_data; }

		protected:
			Type m_type;
			void* m_data;
		};

		template<typename T>
		class HT_API ShaderVariableTemplate : public ShaderVariable
		{
		public:
			inline ShaderVariableTemplate(T t);
		};

		template <>
		HT_API ShaderVariableTemplate<int>::ShaderVariableTemplate(int data)
		{
			memcpy(m_data, &data, sizeof(int));
			m_type = Type::INT;
		}

		template <>
		HT_API ShaderVariableTemplate<float>::ShaderVariableTemplate(float data)
		{
			memcpy(m_data, &data, sizeof(float));
			m_type = Type::FLOAT;
		}

		template <>
		HT_API ShaderVariableTemplate<Math::Vector3>::ShaderVariableTemplate(Math::Vector3 data)
		{
			memcpy(m_data, data.m_data, sizeof(float) * 3);
			m_type = Type::FLOAT3;
		}

		template <>
		HT_API ShaderVariableTemplate<Math::Vector4>::ShaderVariableTemplate(Math::Vector4 data)
		{
			memcpy(m_data, data.data, sizeof(float) * 4);
			m_type = Type::FLOAT4;
		}

		template <>
		HT_API ShaderVariableTemplate<Math::Matrix4>::ShaderVariableTemplate(Math::Matrix4 data)
		{
			memcpy(m_data, data.data, sizeof(float) * 16);
			m_type = Type::MAT4;
		}

		typedef ShaderVariableTemplate<int>				IntVariable;
		typedef ShaderVariableTemplate<float>			FloatVariable;
		typedef ShaderVariableTemplate<Math::Vector3>	Float3Variable;
		typedef ShaderVariableTemplate<Math::Vector4>	Float4Variable;
		typedef ShaderVariableTemplate<Math::Matrix4>	Matrix4Variable;
	}
}
