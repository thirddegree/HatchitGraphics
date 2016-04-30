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

    namespace Graphics {

        class TextureBase;

        class HT_API Texture : public Core::RefCounted<Texture>
        {
        public:
            Texture(Core::Guid ID);

            ~Texture();
        
            bool Initialize(const std::string& file);
            bool InitializeAsync(Core::Handle<Texture> tempHandle, Core::Handle<Texture> defaultHandle, const std::string& file);

            uint32_t            GetWidth()  const;
            uint32_t            GetHeight() const;
            TextureBase* const  GetBase();

        private:
            TextureBase* m_base;
        };

        using TextureHandle = Core::Handle<Texture>;
    }
}
