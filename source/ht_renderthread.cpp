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

#include <ht_renderthread.h>
#include <condition_variable>

namespace Hatchit 
{
    namespace Graphics 
    {
        void RenderThread::Kill() 
        {
            m_alive = false;
            m_processing = false;
            if (m_thread.joinable())
                m_thread.join();
        }

        void RenderThread::Notify() 
        {
            m_waitLock.notify_one();
        }

        const std::atomic_bool& RenderThread::Processed() const
        {
            return !m_processing;
        }
    }
}
