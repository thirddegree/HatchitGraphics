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

#include <ht_vkrenderthread.h>
#include <ht_vkdevice.h>
#include <condition_variable>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKRenderThread::VKRenderThread(VKDevice* device)
            {
                m_device = device->GetVKDevices()[0];

                m_alive = false;
                m_processing = false;
            }

            VKRenderThread::~VKRenderThread() 
            {
                Kill();
            }

            void VKRenderThread::VStart(std::condition_variable* notifyLock, Core::ThreadsafeQueue<RenderPassHandle>* jobQueue)
            {
                m_jobQueue = jobQueue;

                m_notifyLock = notifyLock;

                m_alive = true;

                m_thread = std::thread(&VKRenderThread::thread_main, this);
            }

            void VKRenderThread::thread_main() 
            {
                //Create a command pool for this thread
                VKCommandPool commandPool(m_device);
                if (!commandPool.VInitialize())
                {
                    HT_ERROR_PRINTF("VKRenderThread::thread_main(): Failed to initialize vulkan command pool!\n");
                    Kill();
                    return;
                }

                while (m_alive)
                {
                    //Wait until there is a job that has been added to the queue
                    //std::unique_lock<std::mutex> lock(m_mutex);
                    //m_waitLock.wait(lock, [this] {return !m_jobQueue->empty(); });

                    //This will block if the queue is empty
                    std::shared_ptr<RenderPassHandle> pass = m_jobQueue->wait_pop();
                    m_processing = true;
                    (*pass)->BuildCommandList(&commandPool);
                    m_processing = false;
                    
                    //We've build the command lists; we should be all done
                    //Notify the main thread that we're done
                    m_notifyLock->notify_one();
                }

                //Pool should delete as the thread ends
            }
        }
    }
}