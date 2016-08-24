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
#include <ht_math.h>
#include <ht_shadervariable.h>
#include <ht_shadervariablechunk.h>
#include <ht_texture.h>
#include <ht_pipeline.h>
#include <ht_refcounted.h>
#include <map>

namespace Hatchit {

    namespace Core
    {
        template<typename VarType>
        class Handle;
    }

    namespace Resource
    {
        class Material;
    }

    namespace Graphics {


        struct LayoutLocation {
            uint32_t set;
            uint32_t binding;
        };

        class RenderPass;

        class HT_API MaterialBase
        {
        public:
            MaterialBase();
            ~MaterialBase();

            bool SetInt(size_t chunk, size_t offset, int data);
            bool SetFloat(size_t chunk, size_t offset, float data);
            bool SetFloat3(size_t chunk, size_t offset, Math::Vector3 data);
            bool SetFloat4(size_t chunk, size_t offset, Math::Vector4 data);
            bool SetMatrix4(size_t chunk, size_t offset, Math::Matrix4 data);

            virtual bool VBindTexture(std::string name, TextureHandle texture) = 0;
            virtual bool VUnbindTexture(std::string name, TextureHandle texture) = 0;

            virtual bool VUpdate() = 0;

            virtual PipelineHandle const VGetPipeline() const = 0;

        protected:
            std::vector<Core::Handle<RenderPass>> m_renderPasses;

            std::vector<LayoutLocation> m_shaderVariableLocations;
            std::vector<ShaderVariableChunk*> m_shaderVariables;

            friend class Material;
        };
    }
}
