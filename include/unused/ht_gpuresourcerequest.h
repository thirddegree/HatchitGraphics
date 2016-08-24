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
#include <ht_refcounted.h>

namespace Hatchit
{
    namespace Graphics
    {
        class Texture;
        class Material;
        class RootLayout;
        class Pipeline;
        class Shader;
        class RenderPass;
        class RenderTarget;
        class Mesh;

        class GPUResourceRequest
        {
        public:
            virtual ~GPUResourceRequest() { };

            enum class Type
            {
                Texture,
                Material,
                RootLayout,
                Pipeline,
                Shader,
                RenderPass,
                RenderTarget,
                Mesh
            };

            Type type;
        };

        template <typename T>
        class HT_API GPURequest : public GPUResourceRequest
        {
        public:
            std::string         file;
            void**              data;
        };

        using TextureRequest = GPURequest<Texture>;
        using MaterialRequest = GPURequest<Material>;
        using RootLayoutRequest = GPURequest<RootLayout>;
        using PipelineRequest = GPURequest<Pipeline>;
        using ShaderRequest = GPURequest<Shader>;
        using RenderPassRequest = GPURequest<RenderPass>;
        using RenderTargetRequest = GPURequest<RenderTarget>;
        using MeshRequest = GPURequest<Mesh>;
    }
}
