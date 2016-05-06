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


#include <ht_material_base.h>
#include <ht_shadervariablechunk.h>

namespace Hatchit {

    namespace Graphics {
        MaterialBase::MaterialBase() {};
        MaterialBase::~MaterialBase()
        {
            for (int i = 0; i < m_shaderVariables.size(); i++)
            {
                delete m_shaderVariables[i];
            }
        };

        bool MaterialBase::SetInt(size_t chunk, std::string name, int data)
        {
            m_shaderVariables[chunk]->SetInt(name, data);
            return true;
        }

        bool MaterialBase::SetFloat(size_t chunk, std::string name, float data)
        {
            m_shaderVariables[chunk]->SetFloat(name, data);
            return true;
        }

        bool MaterialBase::SetFloat3(size_t chunk, std::string name, Math::Vector3 data)
        {
            m_shaderVariables[chunk]->SetFloat3(name, data);
            return true;
        }

        bool MaterialBase::SetFloat4(size_t chunk, std::string name, Math::Vector4 data)
        {
            m_shaderVariables[chunk]->SetFloat4(name, data);
            return true;
        }

        bool MaterialBase::SetMatrix4(size_t chunk, std::string name, Math::Matrix4 data)
        {
            m_shaderVariables[chunk]->SetMatrix4(name, data);
            return true;
        }
    }

}