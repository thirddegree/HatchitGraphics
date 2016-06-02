
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


        /**
        *   \fn GPUResourceThread::Load()
        *   \brief Function processes a non-async GPUResourceRequest
        *   \param request Pointer to GPUResourceRequest object
        *
        *   This function will process a GPUResourceRequest non-asynchronously
        *   by pushing the request onto the request stack and then blocking the main thread
        *   for processing.
        */
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

        /**
        *   \fn GPUResourceThread::LoadAsync()
        *   \brief Function processes an async GPUResourceRequest
        *   \param request Pointer to GPUResourceRequet object
        *
        *   This function will process a GPUResourceRequest asynchronously
        *   by pushing the request onto the request stack. It does NOT
        *   block the main thread, but loads asynchronously instead.
        */
        void GPUResourceThread::LoadAsync(GPUResourceRequest* request)
        {
            if (!m_alive)
                VStart();

            m_requests.push(request);
        }

        /**
        *   \fn GPUResourceThread::Kill()
        *   \brief Function kills the thread
        *
        *   This function will kill the thread and join until finished.
        */
        void GPUResourceThread::Kill()
        {
            m_alive = false;
            if (m_thread.joinable())
                m_thread.join();
        }

        /**
        *   \fn GPUResourceThread::CreateTexture()
        *   \brief Function creates a texture
        *   \param file Path of texture file to load.
        *   \param data Pointer to the base texture implementation to fill.
        *   
        *   This function will create a texture object on the GPU resource thread.
        */
        void GPUResourceThread::CreateTexture(std::string file, void ** data)
        {
            Resource::TextureHandle handle = Resource::Texture::GetHandle(file, file);

            VCreateTextureBase(handle, data);
        }
        
        /**
        *   \fn GPUResourceThread::CreateMaterial()
        *   \brief Function creates a material
        *   \param file Path of material file to load.
        *   \param data Pointer to the base material implementation to load.
        *
        *   This function will create a materia object on the GPU resource thread.
        */
        void GPUResourceThread::CreateMaterial(std::string file, void ** data)
        {
            Resource::MaterialHandle handle = Resource::Material::GetHandle(file, file);

            VCreateMaterialBase(handle, data);
        }

        /**
        *   \fn GPUResourceThread::CreateRootLayout()
        *   \brief Function creates a rootlayout
        *   \param file Path to rootlayout file to load.
        *   \param data Pointer to the base rootlayout implementation to fill.
        *
        *   This function will create a rootlayout object on the GPU resource thread.
        */
        void GPUResourceThread::CreateRootLayout(std::string file, void ** data)
        {
            Resource::RootLayoutHandle handle = Resource::RootLayout::GetHandle(file, file);

            VCreateRootLayoutBase(handle, data);
        }

        /**
        *   \fn GPUResourceThread::CreatePipeline()
        *   \brief Function creates a pipeline
        *   \param file Path to pipeline file to load.
        *   \param data Pointer to base pipeline implementation to fill.
        *
        *   This function will create a pipeline object on the GPU resource thread.
        */
        void GPUResourceThread::CreatePipeline(std::string file, void ** data)
        {
            Resource::PipelineHandle handle = Resource::Pipeline::GetHandle(file, file);

            VCreatePipelineBase(handle, data);
        }

        /**
        *   \fn GPUResourceThread::CreateShader()
        *   \brief Function creates a shader
        *   \param file Path to shader file to load.
        *   \param data Pointer to the base shader implementation to fill.
        *
        *   This function will create a shader object on the GPU resource thread.
        */
        void GPUResourceThread::CreateShader(std::string file, void ** data)
        {
            Resource::ShaderHandle handle = Resource::Shader::GetHandle(file, file);

            VCreateShaderBase(handle, data);
        }

        /**
        *   \fn GPUResourceThread::CreateRenderPass()
        *   \brief Function creates a renderpass
        *   \param file Path to renderpass file to load.
        *   \param data Pointer to the base renderpass implementation to fill.
        *
        *   This function will create a renderpass object on the GPU resource thread.
        */
        void GPUResourceThread::CreateRenderPass(std::string file, void ** data)
        {
            Resource::RenderPassHandle handle = Resource::RenderPass::GetHandle(file, file);

            VCreateRenderPassBase(handle, data);
        }

        /**
        *   \fn GPUResourceThread::CreateRenderTarget()
        *   \brief Function creates a rendertarget
        *   \param file Path to rendertarget file to load.
        *   \param data Pointer to the base rendertarget implementation to fill.
        *
        *   This function will create a rendertarget object on the GPU resource thread.
        */
        void GPUResourceThread::CreateRenderTarget(std::string file, void ** data)
        {
            Resource::RenderTargetHandle handle = Resource::RenderTarget::GetHandle(file, file);

            VCreateRenderTargetBase(handle, data);
        }

        /**
        *   \fn GPUResourceThread::CreateMesh()
        *   \brief Function creates a mesh
        *   \param file Path to the mesh file to load.
        *   \param data Pointer to the base mesh implementation to fill.
        *
        *   This function will create a mesh object on the GPU resource thread.
        */
        void GPUResourceThread::CreateMesh(std::string file, void ** data)
        {
            Resource::ModelHandle handle = Resource::Model::GetHandle(file, file);

            VCreateMeshBase(handle, data);
        }


        /**
        *   \fn GPUResourceThread::ProcessTextureRequest()
        *   \brief Function processes a texture request
        *   \param request Pointer to TextureRequest.
        *
        *   This function will process a TextureRequest and create the texture
        *   either non-async or async depending on the state of the GPU resource thread.
        */
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

        /**
        *   \fn GPUResourceThread::ProcessMaterialRequest()
        *   \brief Function processes a material request
        *   \param request Pointer to MaterialRequest.
        *
        *   This function will process a MaterialRequest and create the material
        *   either non-async or async depending on the state of the GPU resource thread.
        */
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

        /**
        *   \fn GPUResourceThread::ProcessRootLayoutRequest()
        *   \brief Function processes a rootlayout request
        *   \param request Pointer to RootLayoutRequest.
        *
        *   This function will process a RootLayoutRequest and create the rootlayout
        *   either non-async or asycn depending on the state of the GPU resource thread.
        */
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

        /**
        *   \fn GPUResourceThread::ProcessPipelineRequest()
        *   \brief Function processes a pipeline request
        *   \param request Pointer to PipelineRequest
        *
        *   This function will process a PipelineRequst and create the pipeline
        *   either non-async or asycn depending on the state of the GPU resource thread.
        */
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

        /**
        *   \fn GPUResourceThread::ProcessShaderRequest()
        *   \brief Function processes a shader request
        *   \param request Pointer to ShaderRequest
        *
        *   This function will process a ShaderRequest and create the shader
        *   either non-async or async depending on the state of the GPU resource thread.
        */
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

        /**
        *   \fn GPUResourceThread::ProcessRenderPassRequest()
        *   \brief Function processes a renderpass request
        *   \param request Pointer to RenderPassRequest
        *
        *   This function will process a RenderPassRequest and create the renderpass
        *   either non-async or async depending on the state of the GPU resource thread.
        */
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
        
        /**
        *   \fn GPUResourceThread::ProcessRenderTargetRequest()
        *   \brief Function processes a rendertarget request
        *   \param request Pointer to RenderTargetRequest
        *
        *   This function will process a RenderTargetRequest and create the rendertarget
        *   either non-async or async depending on the state of the GPU resource thread.
        */
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

        /**
        *   \fn GPUResourceThread::ProcessMeshRequest()
        *   \brief Function processes a mesh request
        *   \param request Pointer to MeshRequest
        *
        *   This function will process a MeshRequest and create the mesh
        *   either non-async or async depending on the state of the GPU resource thread.
        */
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
