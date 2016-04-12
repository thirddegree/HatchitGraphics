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

#pragma once

#include <ht_debug.h>
#include <ht_platform.h>
#include <ht_sampler.h>
#include <cassert>

namespace Hatchit {

    namespace Core
    {
        template<typename VarType>
        class Handle;
    }

    namespace Resource
    {
        class Texture;
    }

    namespace Graphics {

        class HT_API ITexture
        {
        public:
            virtual ~ITexture() { }
        
            virtual void SetSampler(ISamplerHandle sampler) = 0;

            virtual uint32_t GetWidth()  const = 0;
            virtual uint32_t GetHeight() const = 0;

        protected:
            virtual bool VBufferImage() = 0;
        };

        using ITextureHandle = Core::Handle<ITexture>;
    }
}
