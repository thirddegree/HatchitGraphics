/**
**    Hatchit Engine
**    Copyright(c) 2015 ThirdDegree
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

/**
 * \class IPipeline
 * \ingroup HatchitGraphics
 *
 * \brief An interface to a pipeline that should be extended and implemented by a graphics language
 *
 * A pipeline contains all sort of information about how the renderer should render subsequent
 * render passes. This includes things like topology, MSAA, shaders etc.
 *
 * Shader variables set in this class will be sent via PushConstants in Vulkan.
 */
    
#pragma once
    
#include <ht_platform.h>
#include <ht_shader.h>
#include <ht_debug.h>
#include <ht_shadervariablechunk.h>
#include <ht_pipeline_resource.h>
#include <ht_shader_resource.h>

#include <map>
    
namespace Hatchit {
    
   namespace Graphics {
   
        class PipelineBase;
       
        class HT_API Pipeline : public Core::RefCounted<Pipeline>
        {
        public:
            Pipeline(Core::Guid ID);
            
            ~Pipeline();
           
            bool Initialize(const std::string& file);

            /* Add a map of existing shader variables into this pipeline
            * \param shaderVariables the map of existing shader variables you want to add
            */
            bool SetShaderVariables(ShaderVariableChunk* variables);
            
            bool SetInt(size_t offset, int data);
            bool SetDouble(size_t offset, double data);
            bool SetFloat(size_t offset, float data);
            bool SetFloat2(size_t offset, Math::Vector2 data);
            bool SetFloat3(size_t offset, Math::Vector3 data);
            bool SetFloat4(size_t offset, Math::Vector4 data);
            bool SetMatrix4(size_t offset, Math::Matrix4 data);

            ///Update the pipeline after you've changed the uniform data
            bool Update();

            PipelineBase* const GetBase() const;

        protected:
            PipelineBase* m_base;
        };
       
        using PipelineHandle = Core::Handle<Pipeline>;
    }
}