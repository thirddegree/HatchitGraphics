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
#include <ht_device.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            /**
             * \class VKDevice
             * \brief Vulkan device wrapper
             *
             * This class wraps the functionality associated with interfacing with a GPU device
             * using Vulkan. Since there can be multiple active devices, this class represents a single
             * device instance.
             */
            class VKDevice : public IDevice
            {
            public:
                VKDevice();

                ~VKDevice();

                bool VInitialize()          override;
                void VReportDeviceInfo()    override;

            private:
            };
        }
    }
}
