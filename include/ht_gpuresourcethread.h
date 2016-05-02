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

#include <ht_platform.h>
#include <ht_string.h>
#include <ht_threadvector.h>
#include <ht_threadstack.h>
#include <ht_threadqueue.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Hatchit
{
    namespace Graphics
    {
        class GPUResourceRequest;

        class HT_API GPUResourceThread
        {
            using GPURequestQueue = Core::ThreadsafeStack<GPUResourceRequest*>;

        public:
            virtual ~GPUResourceThread() { };
            virtual void VStart() = 0;
            bool Locked() const;
            void Load(GPUResourceRequest* request);
            void LoadAsync(GPUResourceRequest* request);
            void Kill();

        protected:
            std::thread             m_thread;
            std::atomic_bool        m_alive;
            std::atomic_bool        m_tfinished;
            std::atomic_bool        m_locked;
            mutable std::mutex      m_mutex;
            std::condition_variable m_cv;
            std::atomic_bool        m_processed;
            GPURequestQueue         m_requests;
        };
    }
}