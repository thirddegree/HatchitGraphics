/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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


#include <ht_material_base.h>       //MaterialBase
#include <ht_renderpass.h>          //RenderPass
#include <ht_shadervariablechunk.h> //ShaderVariableChunk

namespace Hatchit {

    namespace Graphics {
        
        MaterialBase::MaterialBase() 
        {

        }
        
        MaterialBase::~MaterialBase()
        {
            for (size_t i = 0; i < m_shaderVariables.size(); i++)
            {
                delete m_shaderVariables[i];
            }
        }

        /** Sets an integer that should be sent to the Material's pipeline
        *   when the material is bound
        * 
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool MaterialBase::SetInt(size_t chunk, size_t offset, int data)
        {
            m_shaderVariables[chunk]->SetInt(offset, data);
            return true;
        }

        /** Sets a float that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool MaterialBase::SetFloat(size_t chunk, size_t offset, float data)
        {
            m_shaderVariables[chunk]->SetFloat(offset, data);
            return true;
        }

        /** Sets a Float3/Math::Vector3 that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool MaterialBase::SetFloat3(size_t chunk, size_t offset, Math::Vector3 data)
        {
            m_shaderVariables[chunk]->SetFloat3(offset, data);
            return true;
        }

        /** Sets a Float4/Math::Vector4 that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool MaterialBase::SetFloat4(size_t chunk, size_t offset, Math::Vector4 data)
        {
            m_shaderVariables[chunk]->SetFloat4(offset, data);
            return true;
        }

        /** Sets a Float16/Math::Vector4 that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool MaterialBase::SetMatrix4(size_t chunk, size_t offset, Math::Matrix4 data)
        {
            m_shaderVariables[chunk]->SetMatrix4(offset, data);
            return true;
        }
    }

}
