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
* \class RenderThread
* \ingroup HatchitGraphics
*
* \brief A base class to override in charge of controlling a thread for rendering.
* This should be implemented in a class related to a graphics language.
*/

#pragma once

#include <ht_platform.h>            //HT_API
#include <ht_threadqueue.h>         //Threadsafe Queue
#include <ht_renderpass.h>          //RenderPassHandle
#include <atomic>                   //Atomics
#include <thread>                   //Threads

namespace Hatchit 
{
    namespace Graphics 
    {
        class HT_API RenderThread 
        {
        public:
            virtual ~RenderThread() {};  

            virtual void VStart(std::condition_variable* notifyLock, Core::ThreadsafeQueue<RenderPassHandle>* jobQueue) = 0;
            
            void Kill();
            void Notify();
            std::atomic_bool Processed();

        protected:
            std::thread m_thread;
            std::mutex m_mutex;
            std::condition_variable m_waitLock;     //We wait on this lock before processing
            std::condition_variable* m_notifyLock;  //This is given by the main thread; it's how we notify it that we're done
            std::atomic_bool m_alive;
            std::atomic_bool m_processing;

            Core::ThreadsafeQueue<RenderPassHandle>* m_jobQueue;
        };
    }
}