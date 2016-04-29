
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
#include <ht_gpuresourcethread.h>
#include <ht_threadvector.h>
#include <ht_threadstack.h>
#include <ht_threadqueue.inl>
#include <thread>
#include <atomic>

namespace Hatchit
{
    namespace Graphics
    {
        namespace DX
        {
            class D3D12Device;

            class HT_API D3D12GPUResourceThread : public IGPUResourceThread
            {
                using GPURequestQueue = Core::ThreadsafeQueue<GPUResourceRequest>;
            public:
                D3D12GPUResourceThread(D3D12Device* device);

                ~D3D12GPUResourceThread();

                void VStart()                            override;
                void VLoad(GPUResourceRequest request)   override;
                void VKill()                             override;

            private:
                std::thread             m_thread;
                std::atomic_bool        m_alive;
                std::atomic_bool        m_tfinished;
                D3D12Device*            m_device;
                GPURequestQueue         m_requests;

                void thread_main();
            };
        }
    }
}