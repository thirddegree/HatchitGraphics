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

#include <ht_renderthread.h>    //RenderThread
#include <condition_variable>   //std::condition_variable
#include <thread>               //std::thread

namespace Hatchit 
{
    namespace Graphics 
    {
        /** Kills the render thread
        * 
        * Sets this thread's alive and processing states to false and joins the thread
        * onto the main thread. This should cause the thread to shut down asap.
        */
        void RenderThread::Kill() 
        {
            m_alive = false;
            m_processing = false;
            if (m_thread.joinable())
                m_thread.join();
        }

        /** Notify the thread that it should unlock because it has new 
        *   work to process.
        *
        * Notifies the underlying locking std::condition_variable to wake up
        */
        void RenderThread::Notify() 
        {
            m_waitLock.notify_one();
        }

        /** Gets whether or not the thread has finished processing
        * \return A boolean representing whether or not this thread has finished working.
        */
        const bool RenderThread::Processed() const
        {
            return !m_processing;
        }
    }
}
