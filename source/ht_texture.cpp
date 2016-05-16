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

#include <ht_texture.h>             //Texture
#include <ht_texture_base.h>        //TextureBase
#include <ht_texture_resource.h>    //Resource::TextureResource

#include <ht_gpuresourcepool.h>     //GPUResourcePool

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

        /** Initialize a Texture synchronously with the GPUResourcePool
        *
        * If the GPUResourceThread is already in use the texture will be created directly. 
        * If the thread is not locked we will feed the thread a request.
        * This will LOCK the main thread until it completes.
        *
        * \param file The file path of the Texture that we want to load off the disk
        * \return a boolean representing whether or not this operation succeeded
        */
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

        /** Initialize a Texture asynchronously on the GPUResourceThread
        *
        * NON FUNCTIONING
        * Currently we have not found a solid way to swap data into the temp handle
        *
        * \param tempHandle A handle that will be kept alive and which will be filled with the resulting data
        * \param defaultHandle A handle to a texture that will be used until the texture is created
        * \param file The file path of the texture that we want to load off the disk
        * \return a boolean representing whether or not this operation succeeded
        */
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

        /** Gets the width of the texture
        * 
        * This calls down to the base member's TextureBase::GetWidth method
        *
        * \return Returns the texture width as a uint32_t
        */
        uint32_t Texture::GetWidth() const
        {
            return m_base->m_width;
        }

        /** Gets the height of the texture
        *
        * This calls down to the base member's TextureBase::GetHeight method
        *
        * \return Returns the texture height as a uint32_t
        */
        uint32_t Texture::GetHeight() const
        {
            return m_base->m_height;
        }

        /** Get a pointer to the TextureBase* that this class wraps
        * \return A TextureBase* 
        */
        TextureBase* const Texture::GetBase() const
        {
            return m_base;
        }
    }
}
