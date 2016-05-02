
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

#include <ht_gpuresourcethread.h>

namespace Hatchit
{
    namespace Graphics
    {
        bool GPUResourceThread::Locked() const
        {
            return m_locked;
        }

        void GPUResourceThread::Load(GPUResourceRequest* request)
        {
            if (!m_alive)
                VStart();

            m_processed = false;

            m_requests.push(request);
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() -> bool { m_locked = true;  return this->m_processed; });
            m_locked = false;
        }

        void GPUResourceThread::LoadAsync(GPUResourceRequest* request)
        {
            if (!m_alive)
                VStart();

            m_requests.push(request);
        }

        void GPUResourceThread::Kill()
        {
            m_alive = false;
            if (m_thread.joinable())
                m_thread.join();
        }
    }
}