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
* \class VKCommandPool
* \ingroup HatchitGraphics
*
* \brief A wrapper around a VkCommandPool object that other 
* Hatchit objects can understand abstracted from graphics languages
*/

#pragma once

#include <ht_commandpool.h>
#include <ht_vulkan.h>      //VkCommandPool (the original object from the VK API)
#include <ht_vkdevice.h>    //Need a VKDevice to create a pool

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKCommandPool : public ICommandPool 
            {
            public:
                VKCommandPool(VkDevice device);
                ~VKCommandPool();

                bool VInitialize() override;

                VkCommandPool GetVKCommandPool() const;

            private:
                VkDevice m_device;
                VkCommandPool m_commandPool;
            };
        }
    }
}