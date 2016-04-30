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
            Resource::TextureHandle handle = Resource::Texture::GetHandleFromFileName(file);
            if (!handle.IsValid())
                return false;

#ifdef HT_SYS_WINDOWS
            switch (Renderer::GetType())
            {
                case RendererType::DIRECTX12:
                {
                    m_base = new DX::D3D12Texture;
                    auto base = static_cast<DX::D3D12Texture*>(m_base);
                    if (!base->Initialize(handle, static_cast<DX::D3D12Device*>(Renderer::GetDevice())))
                        return false;

                } break;

                case RendererType::VULKAN:
                {

                } return false;

                default:
                    return false;
            }
#else

#endif
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

        TextureBase* const Texture::GetBase()
        {
            return m_base;
        }
    }
}