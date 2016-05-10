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
* \class VKRenderThread
* \ingroup HatchitGraphics
*
* \brief A class that controls a thread used to render with Vulkan
*/

#pragma once

#include <ht_renderthread.h>
#include <ht_vkdevice.h>
#include <ht_vulkan.h>
#include <ht_vkcommandpool.h>

namespace Hatchit 
{
    namespace Graphics
    {
        namespace Vulkan 
        {
            class HT_API VKRenderThread : public RenderThread
            {
            public:
                VKRenderThread(VKDevice* device);
                ~VKRenderThread();

                void VStart(std::condition_variable* notifyLock, Core::ThreadsafeQueue<RenderPassHandle>* jobQueue)   override;

            private:
                VkDevice        m_device;

                void thread_main();
            };
        }
    }
}