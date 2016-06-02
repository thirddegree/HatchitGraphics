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

#include <ht_material.h>        //Material
#include <ht_material_base.h>   //MaterialBase
#include <ht_renderpass.h>      //RenderPass
#include <ht_gpuresourcepool.h> //GPUResourcePool
#include <ht_guid.h>            //Core::Guid
#include <ht_string.h>          //std::string

namespace Hatchit {

    namespace Graphics {

        Material::Material(Core::Guid ID)
            : Core::RefCounted<Material>(ID)
        {
            m_base = nullptr;
        }

        Material::~Material()
        {
            delete m_base;
        }

        /** Get a reference to this Material's collection of RenderPasses
        * 
        * This collection describes every compatible RenderPass that this Material
        * should be allowed to bind with.
        *
        * \return A reference to a std::vector of RenderPassHandles that this Material is allowed to bind to
        */
        const std::vector<RenderPassHandle>& Material::GetRenderPasses() const 
        {
            return m_base->m_renderPasses;
        }

        /** Get a handle to this Material's Pipeline
        * \return a PipelineHandle of this Material's Pipeline
        */
        PipelineHandle const Material::GetPipeline() const
        {
            return m_base->VGetPipeline();
        }

        /** Initialize a Material synchronously with the GPUResourcePool
        *
        * If the GPUResourceThread is already in use the texture will be created directly.
        * If the thread is not locked we will feed the thread a request.
        * This will LOCK the main thread until it completes.
        *
        * \param file The file path of the Material json file that we want to load off the disk
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Material::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the material base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateMaterial(file, reinterpret_cast<void**>(&m_base));
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

        /** Sets an integer that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Material::SetInt(size_t chunk, size_t offset, int data)
        {
            return m_base->SetInt(chunk, offset, data);
        }

        /** Sets a float that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Material::SetFloat(size_t chunk, size_t offset, float data)
        {
            return m_base->SetFloat(chunk, offset, data);
        }

        /** Sets a Float3/Math::Vector3 that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Material::SetFloat3(size_t chunk, size_t offset, Math::Vector3 data)
        {
            return m_base->SetFloat3(chunk, offset, data);
        }

        /** Sets a Float4/Math::Vector4 that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Material::SetFloat4(size_t chunk, size_t offset, Math::Vector4 data)
        {
            return m_base->SetFloat4(chunk, offset, data);
        }

        /** Sets a Float16/Math::Vector4 that should be sent to the Material's pipeline
        *   when the material is bound
        *
        * \param chunk The index describing which ShaderVariableChunk this data should be placed in
        * \param offset The offset describing how far into the chunk the data should be placed
        * \param data The data to actually insert into the material's shader info
        * \return A boolean representing whether or not this operation succeeded
        */
        bool Material::SetMatrix4(size_t chunk, size_t offset, Math::Matrix4 data)
        {
            return m_base->SetMatrix4(chunk, offset, data);
        }

        /** Binds a texture to the Material with a given name
        * \param name The name of the texture point you want to bind to
        * \param texture A handle to the texture you want to bind into the material
        * \return A boolean representing whether or not the operation succeeded
        */
        bool Material::BindTexture(std::string name, TextureHandle texture)
        {
            return m_base->VBindTexture(name, texture);
        }

        /** Unbinds a texture from the Material with a given name
        * \param name The name of the texture point you want to unbind from
        * \param texture A handle to the texture you want to unbind from the material
        * \return A boolean representing whether or not the operation succeeded
        */
        bool Material::UnbindTexture(std::string name, TextureHandle texture)
        {
            return m_base->VUnbindTexture(name, texture);
        }

        /** Update the Material's GPU-side data
        * 
        * Reorganizes and re-maps the Material's data into its GPU-side 
        * representation. This usually consists of updating the mapped chunk 
        * of GPU-side memory
        */
        bool Material::Update()
        {
            return m_base->VUpdate();
        }

        /** Get a pointer to the MaterialBase that this wraps
        * \return A pointer a MaterialBase object that this object wraps
        */
        MaterialBase* const Material::GetBase() const 
        {
            return m_base;
        }

    }

}