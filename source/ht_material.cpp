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

#include <ht_material.h> //material base
#include <ht_renderer.h>
#include <ht_renderpass.h> //render pass base handle
#include <ht_material_base.h>
#include <ht_gpuresourcepool.h>

namespace Hatchit {

    namespace Graphics {

        //TODO: This should probably just be inlined?
        const std::vector<RenderPassBaseHandle>& Material::GetRenderPasses() const 
        {
            return m_base->m_renderPasses;
        }

        Material::Material(Core::Guid ID)
            : Core::RefCounted<Material>(ID)
        {
            m_base = nullptr;
        }

        Material::~Material()
        {

        }


        bool Material::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");
            }
            else
            {
                //Request texture immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestMaterial(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        bool Material::SetInt(std::string name, int data)
        {
            return m_base->VSetInt(name, data);
        }

        bool Material::SetFloat(std::string name, float data)
        {
            return m_base->VSetFloat(name, data);
        }

        bool Material::SetFloat3(std::string name, Math::Vector3 data)
        {
            return m_base->VSetFloat3(name, data);
        }

        bool Material::SetFloat4(std::string name, Math::Vector4 data)
        {
            return m_base->VSetFloat4(name, data);
        }

        bool Material::SetMatrix4(std::string name, Math::Matrix4 data)
        {
            return m_base->VSetMatrix4(name, data);
        }

        bool Material::BindTexture(std::string name, TextureHandle texture)
        {
            return m_base->VBindTexture(name, texture);
        }
        bool Material::UnbindTexture(std::string name, TextureHandle texture)
        {
            return m_base->VUnbindTexture(name, texture);
        }

        bool Material::Update()
        {
            return m_base->VUpdate();
        }

        IPipelineHandle Material::GetPipeline() {
            return IPipelineHandle();
        }

    }

}