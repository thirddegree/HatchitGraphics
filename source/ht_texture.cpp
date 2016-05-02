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

#include <ht_texture.h>
#include <ht_texture_base.h>
#include <ht_texture_resource.h>
#include <ht_renderer.h>

#include <ht_gpuresourcepool.h>

#ifdef HT_SYS_WINDOWS
#include <ht_d3d12device.h>
#include <ht_d3d12texture.h>
#else

#endif

namespace Hatchit {

    namespace Graphics {

        Texture::Texture(Core::Guid ID)
            : Core::RefCounted<Texture>(ID)
        {
            m_base = nullptr;
        }

        Texture::~Texture()
        {
            delete m_base;
        }

        bool Texture::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the texture base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateTexture(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request texture immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestTexture(file, reinterpret_cast<void**>(&m_base));
            }
            
            return true;
        }

        bool Texture::InitializeAsync(Core::Handle<Texture> tempHandle, Core::Handle<Texture> defaultHandle, const std::string & file)
        {
            //Request texture asynchronously. This will keep the current default alive,
            //while it is loading the resource on the GPUResourceThread. Once
            //loading is finished, the handle for this texture should contain
            //the internal data pointer to the requested resource
            GPUResourcePool::RequestTextureAsync(defaultHandle, tempHandle, file,
                reinterpret_cast<void**>(&m_base));

            return true;
        }

        uint32_t Texture::GetWidth() const
        {
            return m_base->m_width;
        }

        uint32_t Texture::GetHeight() const
        {
            return m_base->m_height;
        }

        TextureBase* const Texture::GetBase() const
        {
            return m_base;
        }
    }
}