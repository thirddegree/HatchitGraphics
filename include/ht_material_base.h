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
#include <ht_texture.h>
#include <ht_pipeline.h>
#include <ht_refcounted.h>

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

        class RenderPass;

        class HT_API MaterialBase
        {
        public:
            virtual ~MaterialBase() {};

            virtual bool VSetInt(std::string name, int data) = 0;
            virtual bool VSetFloat(std::string name, float data) = 0;
            virtual bool VSetFloat3(std::string name, Math::Vector3 data) = 0;
            virtual bool VSetFloat4(std::string name, Math::Vector4 data) = 0;
            virtual bool VSetMatrix4(std::string name, Math::Matrix4 data) = 0;

            virtual bool VBindTexture(std::string name, TextureHandle texture) = 0;
            virtual bool VUnbindTexture(std::string name, TextureHandle texture) = 0;

            virtual bool VUpdate() = 0;

            virtual PipelineHandle VGetPipeline() = 0;
        protected:
            std::vector<Core::Handle<RenderPass>> m_renderPasses;

            friend class Material;
        };
    }
}
