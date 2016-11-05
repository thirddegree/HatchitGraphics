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
#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        class VKDevice;

        namespace Vulkan
        {
            class HT_API VKImage
            {
            public:
                VKImage();

                ~VKImage();

                bool Initialize(VKDevice& device,
                    const VkImageCreateInfo* pImageInfo, 
                    VkImageViewCreateInfo* pImageViewInfo);

                bool AllocateAndBindMemory(VKDevice& pDevice);

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