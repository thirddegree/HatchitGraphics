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
#include <ht_vulkan.h>
#include <ht_vkimage.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;

            class HT_API VKDepthStencil
            {
            public:
                VKDepthStencil();

                ~VKDepthStencil();

                bool Initialize(VKDevice& device, uint32_t width, uint32_t height);

                VKImage& GetImage() const;

            private:
                VkDevice                  m_vkDevice;
                std::unique_ptr<VKImage>  m_buffer;
            };
        }
    }
}