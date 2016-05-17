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

#pragma once

#include <ht_platform.h>
#include <ht_string.h>
#include <ht_threadvector.h>
#include <ht_threadstack.h>
#include <ht_threadqueue.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <ht_gpuresourcerequest.h>

#include <ht_texture_resource.h>
#include <ht_material_resource.h>
#include <ht_rootlayout_resource.h>
#include <ht_pipeline_resource.h>
#include <ht_shader_resource.h>
#include <ht_renderpass_resource.h>
#include <ht_rendertarget_resource.h>
#include <ht_mesh_resource.h>
#include <ht_model.h>

namespace Hatchit
{
    namespace Graphics
    {
        class GPUResourceRequest;

        /**
        *   \class GPUResourceThread
        *   \ingroup HatchitGraphics
        *
        *   \brief Abstract class that defines and implements functions for creation of GPU resource objects.
        *
        *   This class defines the logic for the GPU resource thread owned by GPUResourcePool, for the
        *   loading and creation of GPU resource objects.
        */
        class HT_API GPUResourceThread
        {
            using GPURequestQueue = Core::ThreadsafeStack<GPUResourceRequest*>;

        public:
            virtual ~GPUResourceThread() { };

            virtual void VStart() = 0;

            bool Locked() const;
            void Load(GPUResourceRequest* request);
            void LoadAsync(GPUResourceRequest* request);
            void Kill();

            void CreateTexture(std::string file, void** data);
            void CreateMaterial(std::string file, void** data);
            void CreateRootLayout(std::string file, void** data);
            void CreatePipeline(std::string file, void** data);
            void CreateShader(std::string file, void** data);
            void CreateRenderPass(std::string file, void** data);
            void CreateRenderTarget(std::string file, void** data);
            void CreateMesh(std::string file, void** data);

        protected:
            std::thread             m_thread;
            std::atomic_bool        m_alive;
            std::atomic_bool        m_tfinished;
            std::atomic_bool        m_locked;
            mutable std::mutex      m_mutex;
            std::condition_variable m_cv;
            std::atomic_bool        m_processed;
            GPURequestQueue         m_requests;

            void ProcessTextureRequest(TextureRequest* request);
            void ProcessMaterialRequest(MaterialRequest* request);
            void ProcessRootLayoutRequest(RootLayoutRequest* request);
            void ProcessPipelineRequest(PipelineRequest* request);
            void ProcessShaderRequest(ShaderRequest* request);
            void ProcessRenderPassRequest(RenderPassRequest* request);
            void ProcessRenderTargetRequest(RenderTargetRequest* request);
            void ProcessMeshRequest(MeshRequest* request);

            virtual void VCreateTextureBase(Resource::TextureHandle handle, void** base) = 0;
            virtual void VCreateMaterialBase(Resource::MaterialHandle handle, void** base) = 0;
            virtual void VCreateRootLayoutBase(Resource::RootLayoutHandle handle, void** base) = 0;
            virtual void VCreatePipelineBase(Resource::PipelineHandle handle, void** base) = 0;
            virtual void VCreateShaderBase(Resource::ShaderHandle handle, void** base) = 0;
            virtual void VCreateRenderPassBase(Resource::RenderPassHandle handle, void** base) = 0;
            virtual void VCreateRenderTargetBase(Resource::RenderTargetHandle handle, void** base) = 0;
            virtual void VCreateMeshBase(Resource::ModelHandle handle, void** base) = 0;
        };
    }
}
