/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 ThirdDegree
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

#include <ht_rootlayout.h>
#include <ht_rootlayout_base.h>
#include <ht_gpuresourcepool.h>

namespace Hatchit
{
    namespace Graphics
    {

        RootLayout::RootLayout(Core::Guid ID) :
            Core::RefCounted<RootLayout>(std::move(ID))
        {
            m_base = nullptr;
        }

        RootLayout::~RootLayout()
        {
            delete m_base;
        }

        bool RootLayout::Initialize(const std::string& file)
        {
            if (GPUResourcePool::IsLocked())
            {
                HT_DEBUG_PRINTF("In GPU Resource Thread.\n");

                //Currenty, we are already in the GPU Resource Thread.
                //So instead of submitting a request to fill the rootlayout base,
                //we should just immediately have the thread fill it for us.
                GPUResourcePool::CreateRootLayout(file, reinterpret_cast<void**>(&m_base));
            }
            else
            {
                //Request rootlayout immediately for main thread of execution
                //This call will block the active thread while the GPUResourcePool
                //allocated the memory
                GPUResourcePool::RequestRootLayout(file, reinterpret_cast<void**>(&m_base));
            }

            return true;
        }

        RootLayoutBase* const RootLayout::GetBase() const
        {
            return m_base;
        }

    }
}
