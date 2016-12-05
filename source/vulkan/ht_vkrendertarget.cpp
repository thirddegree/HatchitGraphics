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

#include <ht_vkrendertarget.h>
#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKRenderTarget::VKRenderTarget()
            {
                m_device = VK_NULL_HANDLE;
                m_gpu = VK_NULL_HANDLE;
            }

            VKRenderTarget::~VKRenderTarget()
            {

            }

            bool VKRenderTarget::Initialize(VKDevice& device)
            {
                m_device = device;
                m_gpu = device;



                return true;
            }
        }
    }
}