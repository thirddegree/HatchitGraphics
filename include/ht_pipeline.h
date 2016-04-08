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
#include <ht_shadervariable.h>
#include <ht_pipeline_resource.h>
#include <ht_shader_resource.h>

#include <map>
    
namespace Hatchit {
    
   namespace Graphics {
   
       class HT_API IPipeline
        {
       public:
            virtual ~IPipeline() {};

            /*
            * Initialize GPU pipeline object from resource file
            * \param handle The handle to loaded pipeline resource file
            */
            virtual bool VInitialize(const Resource::PipelineHandle handle) = 0;
           
            /* Add a map of existing shader variables into this pipeline
            * \param shaderVariables the map of existing shader variables you want to add
            */
            virtual bool VAddShaderVariables(std::map<std::string, Resource::ShaderVariable*> shaderVariables) = 0;

            virtual bool VSetInt(std::string name, int data) = 0;
            virtual bool VSetDouble(std::string name, double data) = 0;
            virtual bool VSetFloat(std::string name, float data) = 0;
            virtual bool VSetFloat2(std::string name, Math::Vector2 data) = 0;
            virtual bool VSetFloat3(std::string name, Math::Vector3 data) = 0;
            virtual bool VSetFloat4(std::string name, Math::Vector4 data) = 0;
            virtual bool VSetMatrix4(std::string name, Math::Matrix4 data) = 0;

            ///Update the pipeline after you've changed the uniform data
            virtual bool VUpdate() = 0;

        protected:
            std::map<std::string, Resource::ShaderVariable*> m_shaderVariables;
        };

       using IPipelineHandle = Core::Handle<IPipeline>;
    }
}