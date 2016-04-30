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

    namespace Graphics {

        class RenderPassBase;
        class MaterialBase;
        
        class HT_API Material : public Core::RefCounted<Material>
        {
        public:
            Material(Core::Guid ID);

            ~Material();

            bool Initialize(const std::string& fileName);
            bool InitializeAsync(Core::Handle<Material> handle);

            bool SetInt(std::string name, int data);
            bool SetFloat(std::string name, float data);
            bool SetFloat3(std::string name, Math::Vector3 data);
            bool SetFloat4(std::string name, Math::Vector4 data);
            bool SetMatrix4(std::string name, Math::Matrix4 data);

            bool BindTexture(std::string name, TextureHandle texture);
            bool UnbindTexture(std::string name, TextureHandle texture);

            bool Update();

            IPipelineHandle GetPipeline();

            const std::vector<Core::Handle<RenderPassBase>>& GetRenderPasses() const;

        protected:
            MaterialBase* m_base;

#ifdef VK_SUPPORT
            friend class Vulkan::VKGPUResourceThread;
#endif
#ifdef DX12_SUPPORT
            friend class D3D12GPUResourceThread;
#endif
        };

        using MaterialHandle = Core::Handle<Material>;
    }
}
