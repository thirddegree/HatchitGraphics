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

/**
 * \file ht_vkimage.h
 * \brief VKImage class definition
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKImage class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;

            /**
            * \class VKImage
            * \brief Vulkan image wrapper
            *
            * This class wraps functionality associated with VkImage and VkImageView
            *
            * NOTE:
            *   Currently there is no support for creation of multiple views for
            *   subresource access to the image. The will have to be an implementation
            *   for this in the future, but for now it is not necessary.
            */
            class HT_API VKImage
            {
            public:
                VKImage();

                ~VKImage();

                bool InitializeImage(VKDevice& device,
                    const VkImageCreateInfo& info);
                bool InitializeView(VKDevice& device,
                    VkImageViewCreateInfo& info);

                bool AllocateAndBindMemory(VKDevice& pDevice);

                const VkImage&      GetImage() const;
                const VkImageView&  GetImageView() const;

                operator VkImage();
                operator VkImageView();
                operator VkDeviceMemory();
            protected:
                VkDevice    m_vkDevice;
                VkImage     m_vkImage;
                VkDeviceMemory m_vkMemory;
                VkImageView m_vkImageView;
            };
        }
    }
}
