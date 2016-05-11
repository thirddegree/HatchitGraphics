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

/**
* \class ICommandPool
* \ingroup HatchitGraphics
*
* \brief An interface to an object that allocates command buffers / command lists
* that will need to be implemented with a graphics language
*/

#pragma once

#include <ht_platform.h>    //HT_API

namespace Hatchit
{
    namespace Graphics
    {
        class HT_API ICommandPool 
        {
        public:
            virtual ~ICommandPool() {}

            virtual bool VInitialize() = 0;
        };
    }
}