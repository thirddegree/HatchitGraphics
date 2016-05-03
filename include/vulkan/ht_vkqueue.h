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

/**
* \class GPUQueue
* \ingroup HatchitGraphics
*
* \brief An abstraction of a Queue used by a graphics language
*
* This a very thing abstraction of a very thin wrapper. Extend this
* only to create a very simple wrapper around your graphics language's
* concept of a queue
*/

#pragma once

#include <ht_platform.h>    //HT_API
#include <ht_vulkan.h>      //General Vulkan
#include <ht_gpuqueue.h>    //Extending this class

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKQueue : public GPUQueue
            {
            public:
                VKQueue(QueueType queueType);
                ~VKQueue();

                bool Initialize(const VkDevice& device, const VkPhysicalDevice& gpu);

                const VkQueue& GetVKQueue() const;

            private:
                VkQueue m_queue;
            };
        }
    }
}