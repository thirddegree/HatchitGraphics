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

#include <ht_shader.h>          //Shader
#include <ht_gpuresourcepool.h> //GPUResourcePool

namespace Hatchit
{
    namespace Graphics
    {
        Shader::Shader(Core::Guid ID)
            : Core::RefCounted<Shader>(ID)
        {
            m_base = nullptr;
        }

        Shader::~Shader()
        {
            delete m_base;
        }

        /** Initialize a Shader with the GPUResourcePool
        *
        * This initialization step takes place on the GPUResourceThread but LOCKS the 
        * main thread. An InitializeAsync method should be used for asynchronous requests. 
        * If the pool is already in use we will assume that the request is made inside the thread
        * and the Shader will be created immediately. 
        *
        * \param file The file path to the shader we want to load from disk
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Shader::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the shader base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateShader(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request shader immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestShader(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        /** Gets a pointer to the ShaderBase that this class wraps
        * \return A poiner to the ShaderBase object that this class wraps.
        */
        ShaderBase* const Shader::GetBase() const
        {
            return m_base;
        }
    }
}