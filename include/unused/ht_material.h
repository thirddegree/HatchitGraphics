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

/**
* \class IMaterial
* \ingroup HatchitGraphics
*
* \brief An interface for a material to draw objects with
*
* This class will be extended by a class that will implement its
* methods with ones that will make calls to a graphics language
*/

#pragma once

#include <ht_platform.h>
#include <ht_shader.h>
#include <ht_texture.h>
#include <ht_shadervariable.h>
#include <ht_math.h>
#include <ht_debug.h>
#include <ht_pipeline.h>
#include <ht_guid.h>

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

            struct LayoutLocation {
                uint32_t set;
                uint32_t binding;
            };

    namespace Graphics {

        class RenderPass;
        class MaterialBase;
        
        class HT_API Material : public Core::RefCounted<Material>
        {
        public:
            Material(Core::Guid ID);

            ~Material();

            bool Initialize(const std::string& file);
            bool InitializeAsync(Core::Handle<Material> handle);

            bool SetInt(size_t chunk, size_t offset, int data);
            bool SetFloat(size_t chunk, size_t offset, float data);
            bool SetFloat3(size_t chunk, size_t offset, Math::Vector3 data);
            bool SetFloat4(size_t chunk, size_t offset, Math::Vector4 data);
            bool SetMatrix4(size_t chunk, size_t offset, Math::Matrix4 data);

            bool BindTexture(std::string name, TextureHandle texture);
            bool UnbindTexture(std::string name, TextureHandle texture);

            bool Update();

            const std::vector<Core::Handle<RenderPass>>& GetRenderPasses() const;
            PipelineHandle const GetPipeline() const;
            MaterialBase* const GetBase() const;

        protected:
            MaterialBase* m_base;

        };

        using MaterialHandle = Core::Handle<Material>;
    }
}
