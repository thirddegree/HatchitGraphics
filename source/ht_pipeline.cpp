/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 ThirdDegree
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

#include <ht_pipeline.h>
#include <ht_gpuresourcepool.h>

namespace Hatchit
{
    namespace Graphics
    {
        Pipeline::Pipeline(Core::Guid ID)
            : Core::RefCounted<Pipeline>(ID)
        {
            m_base = nullptr;
        }

        Pipeline::~Pipeline()
        {
            delete m_base;
        }

        bool Pipeline::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the pipeline base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreatePipeline(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request pipeline immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestPipeline(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        bool Pipeline::AddShaderVariables(std::map<std::string, Resource::ShaderVariable*> shaderVariables)
        {
            return false;
        }

        bool Pipeline::SetInt(std::string name, int data)
        {
            return false;
        }

        bool Pipeline::SetDouble(std::string name, double data)
        {
            return false;
        }

        bool Pipeline::SetFloat(std::string name, float data)
        {
            return false;
        }

        bool Pipeline::SetFloat2(std::string name, Math::Vector2 data)
        {
            return false;
        }

        bool Pipeline::SetFloat3(std::string name, Math::Vector3 data)
        {
            return false;
        }

        bool Pipeline::SetFloat4(std::string name, Math::Vector4 data)
        {
            return false;
        }

        bool Pipeline::SetMatrix4(std::string name, Math::Matrix4 data)
        {
            return false;
        }

        bool Pipeline::Update()
        {
            return false;
        }

        PipelineBase * const Pipeline::GetBase() const
        {
            return m_base;
        }

    }
}