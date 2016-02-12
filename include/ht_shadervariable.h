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

namespace Hatchit {

    namespace Graphics {

        class IShader;

        class HT_API IShaderVariable
        {
        public:
            virtual ~IShaderVariable() { }
            
            virtual void VBind(std::string name, IShader* shader) = 0;
            virtual void VUnbind(std::string name, IShader* shader) = 0;
        };

		class HT_API IntVariable : public IShaderVariable 
		{
		public:
			IntVariable(int val);

			~IntVariable();

			void SetData(int val);
			void VBind(std::string name, IShader* shader);
			void VUnbind(std::string name, IShader* shader);

		private:
			float m_val;
		};

        class HT_API FloatVariable : public IShaderVariable
        {
        public:
            FloatVariable(float val);

            ~FloatVariable();

            void SetData(float val);
            void VBind(std::string name, IShader* shader);
            void VUnbind(std::string name, IShader* shader);

        private:
            float m_val;
        };

        class HT_API Float2Variable : public IShaderVariable
        {
        public:
            Float2Variable(float x, float y);

            ~Float2Variable();

            void SetData(float x, float y);
            void VBind(std::string name, IShader* shader);
            void VUnbind(std::string name, IShader* shader);

        private:
            float m_x;
            float m_y;
        };

        class HT_API Float3Variable : public IShaderVariable
        {
        public:
            Float3Variable(float x, float y, float z);

            ~Float3Variable();

            void SetData(float x, float y, float z);
            void VBind(std::string name, IShader* shader);
            void VUnbind(std::string name, IShader* shader);

        private:
            float m_x;
            float m_y;
            float m_z;
        };

        class HT_API Float4Variable : public IShaderVariable
        {
        public:
            Float4Variable(float x, float y, float z, float w);

            ~Float4Variable();

            void SetData(float x, float y, float z, float w);
            void VBind(std::string name, IShader* shader);
            void VUnbind(std::string name, IShader* shader);

        private:
            float m_x;
            float m_y;
            float m_z;
            float m_w;
        }; 
    }
}
