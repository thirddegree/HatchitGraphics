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

namespace Hatchit
{
    namespace Graphics
    {
        struct GPUResourceRequest
        {
            enum class Type
            {
                Texture
            };

            Type            type;
            std::string     file;
            std::string     dflt;
        };

        class HT_API IGPUResourceThread
        {
        public:
            virtual ~IGPUResourceThread() { };
            virtual void VStart() = 0;
            virtual void VLoad(GPUResourceRequest request) = 0;
            virtual void VKill() = 0;
        };
    }
}