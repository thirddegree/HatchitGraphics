/**
**    Hatchit Engine
**    Copyright(c) 2016 Third-Degree
**
**    Created by Matt Guerrette on 8/23/16.
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
#include <ht_device.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKDevice : public IDevice
            {
            public:
                VKDevice();

                ~VKDevice();

                bool VInitialize()          override;
                void VReportDeviceInfo()    override;
            };
        }
    }
}