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

#include <ht_guid.h>            //Core::Guid
#include <ht_renderpass.h>      //RenderPass
#include <ht_renderpass_base.h> //RenderPassBase
#include <ht_gpuresourcepool.h> //GPUResourcePool
#include <ht_commandpool.h>     //ICommandPool
#include <ht_string.h>          //std::string

namespace Hatchit
{
    namespace Graphics
    {            
        RenderPass::RenderPass(Core::Guid ID):
            Core::RefCounted<RenderPass>(std::move(ID))
        {
            m_base = nullptr;
        }

        /** Initialize a RenderPass synchronously with the GPUResourcePool
        *
        * If the GPUResourceThread is already in use the texture will be created directly.
        * If the thread is not locked we will feed the thread a request.
        * This will LOCK the main thread until it completes.
        *
        * \param file The file path of the RenderPass json file that we want to load off the disk
        * \return A boolean representing whether or not this operation succeeded
        */
        bool RenderPass::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the pipeline base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateRenderPass(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request pipeline immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestRenderPass(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        /** Build a command list with the given command pool
        * 
        * Given an interface to a command pool, record all the necesary
        * commands to render this render pass onto a command list.
        *
        * \param commandPool A pointer to the command pool to build the command list from
        * \return A boolean representing whether or not this operation succeeded
        */
        bool RenderPass::BuildCommandList(const ICommandPool* commandPool) 
        {
            return m_base->VBuildCommandList(commandPool);
        }

        /** Set the view matrix to be used in this render pass
        * \param view The Math::Matrix4 to be used for the view matrix
        */
        void RenderPass::SetView(Math::Matrix4 view)
        {
            m_base->SetView(view);
        }

        /** Set the projection matrix to be used in this render pass
        * \param proj The Math::Matrix4 to be used for the projection matrix
        */
        void RenderPass::SetProj(Math::Matrix4 proj)
        {
            m_base->SetProj(proj);
        }

        /** Schedule a render request on this render pass
        *
        * Provide a material, mesh and any instance data you want and that object will be
        * rendered in a command as part of this pass. The data will be sorted and built later.
        *
        * \param material A handle to the material you want to render with
        * \param mesh A handle to the mesh you want to render
        * \param instanceVariables Any instance level variables required for rendering
        */
        void RenderPass::ScheduleRenderRequest(MaterialHandle material, MeshHandle mesh, ShaderVariableChunk* instanceVariables) 
        {
            m_base->ScheduleRenderRequest(material, mesh, instanceVariables);
        }

        /** Gets the layers that this RenderPassBase is a part of
        * \return A uint64_t bitfield of the layers that this is a part of
        */
        uint64_t RenderPass::GetLayerFlags()
        {
            return m_base->GetLayerFlags();
        }

        RenderPassBase* const RenderPass::GetBase() const
        {
            return m_base;
        }

    }
}