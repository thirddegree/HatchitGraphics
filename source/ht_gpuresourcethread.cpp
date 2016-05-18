
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

#include <ht_gpuresourcethread.h>

#include <ht_texture_resource.h>
#include <ht_material_resource.h>
#include <ht_rootlayout_resource.h>
#include <ht_pipeline_resource.h>
#include <ht_shader_resource.h>

namespace Hatchit
{
    namespace Graphics
    {

        /**
        *   \fn GPUResourceThread::Locked()
        *   \brief Function returns if thread is locked.
        *   \return m_locked True if thread is locked, otherwise false.
        *
        *   The function returns true if the thread is currently in a locked state,
        *   otherwise it returns false.
        *   
        *   NOTE: A GPUResourceThread is usually in a locked state due to a non-async
        *   resource request.
        */
        bool GPUResourceThread::Locked() const
        {
            return m_locked;
        }

        void GPUResourceThread::Load(GPUResourceRequest* request)
        {
            if (!m_alive)
                VStart();

            m_processed = false;

            m_requests.push(request);

            std::unique_lock<std::mutex> lock(m_mutex);
            while(!m_processed)
                m_cv.wait(lock, [this]() -> bool { m_locked = true;  return this->m_processed; });
            
            m_locked = false;
        }

        void GPUResourceThread::LoadAsync(GPUResourceRequest* request)
        {
            if (!m_alive)
                VStart();

            m_requests.push(request);
        }

        void GPUResourceThread::Kill()
        {
            m_alive = false;
            if (m_thread.joinable())
                m_thread.join();
        }

        void GPUResourceThread::CreateTexture(std::string file, void ** data)
        {
            Resource::TextureHandle handle = Resource::Texture::GetHandle(file, file);

            VCreateTextureBase(handle, data);
        }

        void GPUResourceThread::CreateMaterial(std::string file, void ** data)
        {
            Resource::MaterialHandle handle = Resource::Material::GetHandle(file, file);

            VCreateMaterialBase(handle, data);
        }

        void GPUResourceThread::CreateRootLayout(std::string file, void ** data)
        {
            Resource::RootLayoutHandle handle = Resource::RootLayout::GetHandle(file, file);

            VCreateRootLayoutBase(handle, data);
        }

        void GPUResourceThread::CreatePipeline(std::string file, void ** data)
        {
            Resource::PipelineHandle handle = Resource::Pipeline::GetHandle(file, file);

            VCreatePipelineBase(handle, data);
        }

        void GPUResourceThread::CreateShader(std::string file, void ** data)
        {
            Resource::ShaderHandle handle = Resource::Shader::GetHandle(file, file);

            VCreateShaderBase(handle, data);
        }

        void GPUResourceThread::CreateRenderPass(std::string file, void ** data)
        {
            Resource::RenderPassHandle handle = Resource::RenderPass::GetHandle(file, file);

            VCreateRenderPassBase(handle, data);
        }

        void GPUResourceThread::CreateRenderTarget(std::string file, void ** data)
        {
            Resource::RenderTargetHandle handle = Resource::RenderTarget::GetHandle(file, file);

            VCreateRenderTargetBase(handle, data);
        }

        void GPUResourceThread::CreateMesh(std::string file, void ** data)
        {
            Resource::ModelHandle handle = Resource::Model::GetHandle(file, file);

            VCreateMeshBase(handle, data);
        }

        void GPUResourceThread::ProcessTextureRequest(TextureRequest * request)
        {
            Resource::TextureHandle handle = Resource::Texture::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async texture load.\n");

            }
            else
            {
                HT_DEBUG_PRINTF("Non-Async texture load.\n");

                VCreateTextureBase(handle, request->data);
            }
        }

        void GPUResourceThread::ProcessMaterialRequest(MaterialRequest * request)
        {
            Resource::MaterialHandle handle = Resource::Material::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async material load.\n");

            }
            else
            {
                HT_DEBUG_PRINTF("Non-Async material load.\n");

                VCreateMaterialBase(handle, request->data);
            }
        }

        void GPUResourceThread::ProcessRootLayoutRequest(RootLayoutRequest * request)
        {
            Resource::RootLayoutHandle handle = Resource::RootLayout::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async rootlayout load.\n");
            }
            else
            {
                HT_DEBUG_PRINTF("Non-async rootlayout load.\n");

                VCreateRootLayoutBase(handle, request->data);
            }
        }

        void GPUResourceThread::ProcessPipelineRequest(PipelineRequest * request)
        {
            Resource::PipelineHandle handle = Resource::Pipeline::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async pipeline load.\n");
            }
            else
            {
                HT_DEBUG_PRINTF("Non-async pipeline load.\n");

                VCreatePipelineBase(handle, request->data);
            }
        }

        void GPUResourceThread::ProcessShaderRequest(ShaderRequest * request)
        {
            Resource::ShaderHandle handle = Resource::Shader::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async shader load.\n");
            }
            else
            {
                HT_DEBUG_PRINTF("Non-async shader load.\n");

                VCreateShaderBase(handle, request->data);
            }
        }

        void GPUResourceThread::ProcessRenderPassRequest(RenderPassRequest* request)
        {
            Resource::RenderPassHandle handle = Resource::RenderPass::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async render pass load.\n");
            }
            else
            {
                HT_DEBUG_PRINTF("Non-async render pass load.\n");

                VCreateRenderPassBase(handle, request->data);
            }
        }
        void GPUResourceThread::ProcessRenderTargetRequest(RenderTargetRequest* request)
        {
            Resource::RenderTargetHandle handle = Resource::RenderTarget::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async render target load.\n");
            }
            else
            {
                HT_DEBUG_PRINTF("Non-async render target load.\n");

                VCreateRenderTargetBase(handle, request->data);
            }
        }
        void GPUResourceThread::ProcessMeshRequest(MeshRequest* request)
        {
            Resource::ModelHandle handle = Resource::Model::GetHandle(request->file, request->file);
            if (!m_locked)
            {
                HT_DEBUG_PRINTF("Async mesh load.\n");
            }
            else
            {
                HT_DEBUG_PRINTF("Non-async mesh load.\n");

                VCreateMeshBase(handle, request->data);
            }
        }
    }
}
