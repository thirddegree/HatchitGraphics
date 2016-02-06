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

#include <ht_shadervariable.h>
#include <ht_shader.h>

namespace Hatchit {

    namespace Graphics {
        
        ////////////////////////////////////////////////////////////////
        // FloatVariable implementation
        ////////////////////////////////////////////////////////////////
        
        FloatVariable::FloatVariable(float val)
        {
            m_val = val;
        }

        FloatVariable::~FloatVariable()
        {

        }

        void FloatVariable::SetData(float val)
        {
            m_val = val;
        }

        void FloatVariable::VBind(std::string name, IShader* shader)
        {
            if(!shader)
                return;

            shader->VSetFloat(name, m_val);
        }

        void FloatVariable::VUnbind(std::string name, IShader* shader)
        {
            
        }

        /////////////////////////////////////////////////////////////////
        // Float2Variable implementation
        /////////////////////////////////////////////////////////////////
        
        Float2Variable::Float2Variable(float x, float y)
        {
            m_x = x;
            m_y = y;
        }

        Float2Variable::~Float2Variable()
        {

        }

        void Float2Variable::SetData(float x, float y)
        {
            m_x = x;
            m_y = y;
        }

        void Float2Variable::VBind(std::string name, IShader* shader)
        {
            if(!shader)
                return;

            shader->VSetFloat2(name, m_x, m_y);
        }

        void Float2Variable::VUnbind(std::string name, IShader* shader)
        {

        }

        ////////////////////////////////////////////////////////////////
        // Float3Variable implementation
        ////////////////////////////////////////////////////////////////

        Float3Variable::Float3Variable(float x, float y, float z)
        {
            m_x = x;
            m_y = y;
            m_z = z;
        }

        Float3Variable::~Float3Variable()
        {

        }

        void Float3Variable::SetData(float x, float y, float z)
        {
            m_x = x;
            m_y = y;
            m_z = z;
        }

        void Float3Variable::VBind(std::string name, IShader* shader)
        {
            if(!shader)
                return;

            shader->VSetFloat3(name, m_x, m_y, m_z);
        }
 
        void Float3Variable::VUnbind(std::string name, IShader* shader)
        {

        }

        ////////////////////////////////////////////////////////////////
        // Float4Variable implementation
        ////////////////////////////////////////////////////////////////

        Float4Variable::Float4Variable(float x, float y, float z, float w)
        {
            m_x = x;
            m_y = y;
            m_z = z;
            m_w = w;
        }

        Float4Variable::~Float4Variable()
        {

        }

        void Float4Variable::SetData(float x, float y, float z, float w)
        {
            m_x = x;
            m_y = y;
            m_z = z;
            m_w = w;
        }

        void Float4Variable::VBind(std::string name, IShader* shader)
        {
            if(!shader)
                return;
            
            shader->VSetFloat4(name, m_x, m_y, m_z, m_w);
        }
        
        void Float4Variable::VUnbind(std::string name, IShader* shader)
        {

        } 
    }
}
