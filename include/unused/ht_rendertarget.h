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
* \class RenderTarget
* \ingroup HatchitGraphics
*
* \brief A wrapper over a RenderTargetBase
*/

#pragma once

#include <ht_platform.h>
#include <ht_rendertarget_resource.h>

namespace Hatchit {

    namespace Graphics {
        
        class RenderTargetBase;

        class HT_API RenderTarget : public Core::RefCounted<RenderTarget>
        {
        public:
            RenderTarget(Core::Guid ID);
            ~RenderTarget();

            bool Initialize(const std::string& file);

            Resource::RenderTarget::BlendOp GetColorBlendOp() const;
            Resource::RenderTarget::BlendOp GetAlphaBlendOp() const;

            RenderTargetBase* const GetBase() const;

        private:
            RenderTargetBase* m_base;
        };

        using RenderTargetHandle = Core::Handle<RenderTarget>;
    }
}
