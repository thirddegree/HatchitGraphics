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

#ifdef HT_SYS_LINUX
#include <cstring>
#endif

namespace Hatchit {

    namespace Graphics {

        class IShader;

        class HT_API ShaderVariable 
        {
        public:
            virtual ~ShaderVariable() {
                if (m_data != nullptr)
                {
                    delete[] m_data;
                    m_data = nullptr;
                }
            };
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
            void* m_data = nullptr;
        };

        template<typename T>
        class HT_API ShaderVariableTemplate : public ShaderVariable
        {
        public:
            inline ShaderVariableTemplate();
            inline ShaderVariableTemplate(T t);
            
            inline void SetData(T t);
        };


	template<>
        inline void HT_API ShaderVariableTemplate<int>::SetData(int data)
        {
            m_data = new BYTE[sizeof(int)];
            memcpy(m_data, &data, sizeof(int));
            m_type = Type::INT;
        }

        template <>
        inline HT_API ShaderVariableTemplate<int>::ShaderVariableTemplate()
        {
            SetData(0);
        }

        template <>
        inline HT_API ShaderVariableTemplate<int>::ShaderVariableTemplate(int data)
        {
            SetData(data);
        }


	template<>
        inline void HT_API ShaderVariableTemplate<float>::SetData(float data)
        {
            m_data = new BYTE[sizeof(float)];
            memcpy(m_data, &data, sizeof(float));
            m_type = Type::FLOAT;
        }

        template <>
        inline HT_API ShaderVariableTemplate<float>::ShaderVariableTemplate()
        {
            SetData(0.0f);
        }

        template <>
        inline HT_API ShaderVariableTemplate<float>::ShaderVariableTemplate(float data)
        {
            SetData(data);
        }


    template<>
        inline void HT_API ShaderVariableTemplate<double>::SetData(double data)
        {
            m_data = new BYTE[sizeof(double)];
            memcpy(m_data, &data, sizeof(double));
            m_type = Type::DOUBLE;
        }

        template <>
        inline HT_API ShaderVariableTemplate<double>::ShaderVariableTemplate()
        {
            SetData(0.0f);
        }

        template <>
        inline HT_API ShaderVariableTemplate<double>::ShaderVariableTemplate(double data)
        {
            SetData(data);
        }


    template<>
        inline void HT_API ShaderVariableTemplate<Math::Vector2>::SetData(Math::Vector2 data)
        {
            m_data = new BYTE[sizeof(float) * 2];
            memcpy(m_data, data.m_data, sizeof(float) * 2);
            m_type = Type::FLOAT2;
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Vector2>::ShaderVariableTemplate()
        {
            SetData(Math::Vector2());
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Vector2>::ShaderVariableTemplate(Math::Vector2 data)
        {
            SetData(data);
        }
       

	template<>
        inline void HT_API ShaderVariableTemplate<Math::Vector3>::SetData(Math::Vector3 data)
        {
            m_data = new BYTE[sizeof(float) * 3];
            memcpy(m_data, data.m_data, sizeof(float) * 3);
            m_type = Type::FLOAT3;
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Vector3>::ShaderVariableTemplate()
        {
            SetData(Math::Vector3());
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Vector3>::ShaderVariableTemplate(Math::Vector3 data)
        {
            SetData(data);
        }       


	template<>
        inline void HT_API ShaderVariableTemplate<Math::Vector4>::SetData(Math::Vector4 data)
        {
            m_data = new BYTE[sizeof(float) * 4];
            memcpy(m_data, data.data, sizeof(float) * 4);
            m_type = Type::FLOAT4;
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Vector4>::ShaderVariableTemplate()
        {   
            SetData(Math::Vector4());
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Vector4>::ShaderVariableTemplate(Math::Vector4 data)
        {
            SetData(data);
        }


	template<>
        inline void HT_API ShaderVariableTemplate<Math::Matrix4>::SetData(Math::Matrix4 data)
        {
            m_data = new BYTE[sizeof(float) * 16];
            memcpy(this->m_data, data.data, sizeof(float) * 16);
            m_type = Type::MAT4;
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Matrix4>::ShaderVariableTemplate()
        {
            SetData(Math::Matrix4());
        }

        template <>
        inline HT_API ShaderVariableTemplate<Math::Matrix4>::ShaderVariableTemplate(Math::Matrix4 data)
        {
            SetData(data);
        }


        typedef ShaderVariableTemplate<int>             IntVariable;
        typedef ShaderVariableTemplate<float>           FloatVariable;
        typedef ShaderVariableTemplate<double>          DoubleVariable;
        typedef ShaderVariableTemplate<Math::Vector2>   Float2Variable;
        typedef ShaderVariableTemplate<Math::Vector3>   Float3Variable;
        typedef ShaderVariableTemplate<Math::Vector4>   Float4Variable;
        typedef ShaderVariableTemplate<Math::Matrix4>   Matrix4Variable;
    }
}
